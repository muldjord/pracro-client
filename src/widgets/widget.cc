/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            widget.cc
 *
 *  Tue Jul 17 12:15:59 CEST 2007
 *  Copyright 2007 Bent Bisballe Nyeng and Lars Bisballe Jensen
 *  deva@aasimon.org and elsenator@gmail.com
 ****************************************************************************/

/*
 *  This file is part of Pracro.
 *
 *  Pracro is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Pracro is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Pracro; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "widget.h"

#include "macrowindow.h"
#include "luawidget.h"
#include "macrodrawer.h"
#include "../widgets.h"

#include <QLayout>
#include <QObject>
#include <QKeySequence>
#include <QApplication>

#include "debug.h"

Widget::Widget(QDomNode &node, MacroWindow *macrowindow)
{
  widget = nullptr;

  hideChildren = false;

  QDomElement elem = node.toElement();

  this->macrowindow = macrowindow;
  if(macrowindow) {
    this->lua = macrowindow->lua;
  } else {
    this->lua = nullptr;
  }

  widget_type = elem.tagName();

  if(elem.hasAttribute("name")) {
    widget_name = elem.attribute("name");
  }

  widget_local = 
    elem.hasAttribute("local") && elem.attribute("local") == "true";

  has_lazy = elem.hasAttribute("name") && elem.hasAttribute("value");
  lazy_value = elem.attribute("value", "");
  lazy_source = elem.attribute("prefilled", "prefilled");

  hasOnChangeEvent = elem.hasAttribute("onChange");
  onChangeEventScript = elem.attribute("onChange", "");

  hasOnInitEvent = elem.hasAttribute("onInit");
  onInitEventScript = elem.attribute("onInit", "");

  important =
    elem.hasAttribute("important") && elem.attribute("important") == "true";

  shortcut_label = nullptr;
  shortcut_action = nullptr;
  if(elem.hasAttribute("shortcut")) {
    QString shortcut = elem.attribute("shortcut", "");
    DEBUG(shortcut, "New shortcut: %s [%s]\n", 
          widget_name.toStdString().c_str(),
          shortcut.toStdString().c_str());
    shortcut_action = new QAction(widget_name, this);
    shortcut_action->setShortcut(QKeySequence(shortcut));
    //    addAction(a);
    connect(shortcut_action, &QAction::triggered, this, &Widget::shortcutActivated);
  }

  is_valid = true;

  connect(this, &Widget::eventOnChange, this, &Widget::runEventOnChange, Qt::QueuedConnection);

  connect(this, &Widget::eventOnInit, this, &Widget::runEventOnInit, Qt::QueuedConnection);

  replwidget = nullptr;

  DEBUG(widget, "Create Widget '%s' of type '%s'\n",
        name().toStdString().c_str(),
        type().toStdString().c_str());
}

Widget::~Widget()
{
  DEBUG(widget, "Delete Widget '%s' of type '%s'\n",
        name().toStdString().c_str(),
        type().toStdString().c_str());

  QVector< Widget* >::iterator i = children.begin();
  while(i != children.end()) {
    if(*i) delete *i;
    i++;
  }

  children.clear();

  // This MUST be done after the deletion of the children Widget list.
  if(widget) {
    delete widget;
    widget = nullptr;
  }

  if(shortcut_label) delete shortcut_label;
}

void Widget::setWidget(QWidget *widget)
{
  this->widget = widget;
  if(!this->widget) return;

  if(important) {
    this->widget->setProperty("important", QString("true"));
  }

  if(shortcut_action) {
    this->widget->addAction(shortcut_action);

    QString lbl = shortcut_action->shortcut().toString();
    shortcut_label = new ShortcutTooltip(lbl, macrowindow);

    // Catch shortcut label show/hide and ESC for cancel
    macrowindow->drawer->installEventFilter(this);
  }
}

bool Widget::eventFilter(QObject *, QEvent *event)
{
  // The function of this 'p' is to determine if this particular event has
  //  been used to issue a close action.
  // If this is in fact the case then we should not do it gain until a new
  //  event is received.
  // It might be considered a hack, but it works...
  static void *p = nullptr;
  if(p != event) p = nullptr;

  if(event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
    QKeyEvent *kevent = (QKeyEvent*)event;

    if(shortcut_label) {
      bool visible =
        (kevent->modifiers() & Qt::ControlModifier ||
         kevent->modifiers() & Qt::AltModifier) &&
        widget->isVisible();

      shortcut_label->setShow(visible);
      if(visible) {
        shortcut_label->move(widget->mapTo(macrowindow, QPoint(-5,-5)));
      }
    }

    if(event->type() == QEvent::KeyPress && kevent->key() == Qt::Key_Escape) {
      if(!p) { // See comment above.
        macrowindow->cancel();
        p = event;
      }
    }
  }

  return false;
}

QWidget *Widget::getWidget()
{
  return this->widget;
}

QString Widget::name()
{
  return widget_name;
}

QString Widget::type()
{
  return widget_type;
}

bool Widget::local()
{
  return widget_local;
}

bool Widget::committable(bool deep)
{
  if(preCommit() == false) return false;

  if(deep == false) return true;

  QVector< Widget* >::iterator i = children.begin();
  while(i != children.end()) {
    if(*i && (*i)->committable(deep) == false) return false;
    i++;
  }

  return true;
}

bool Widget::valid(bool deep)
{
  if(enabled() == false) return true;
  if(visible() == false) return true;
  
  if(preValid() == false) return false;
  if(is_valid == false) return false;
  
  if(hideChildren && deep == false) return true;

  QVector< Widget* >::iterator i = children.begin();
  while(i != children.end()) {
    if(*i && (*i)->valid() == false) return false;
    i++;
  }

  return true;
}

void Widget::setValid(bool valid, bool deep)
{
  is_valid = valid;
  setWdgValid(valid);

  if(!deep) return;

  for(auto *w: children) {
    if(w) w->setValid(valid, deep);
  }
}

void Widget::runEventOnChange(bool deep)
{
  if(enabled()) {
    //if(preValid() == false) setWdgValid(false);
    setWdgValid(valid());
    if(hasOnChangeEvent)
      if(lua) lua->runScript(onChangeEventScript, this, "onChange");
  }

  if(!deep) return;
  
  for(auto *w: children) {
    if(w) w->runEventOnChange(deep);
  }
}

void Widget::runEventOnInit(bool deep)
{
  if(enabled()) {
    //if(preValid() == false) setWdgValid(false);
    setWdgValid(valid());
    if(hasOnInitEvent)
      if(lua) lua->runScript(onInitEventScript, this, "onInit");
  }

  if(!deep) return;
  
  for(auto *w: children) {
    if(w) w->runEventOnInit(deep);
  }
}

void Widget::setWdgValidRecursive(bool forcevalid)
{
  if(forcevalid) setWdgValid(true);
  else setWdgValid(valid());

  for(auto *w: children) {
    if(w) w->setWdgValidRecursive(forcevalid);
  }
}

void Widget::setEnabled(bool enabled)
{
  widget->setEnabled(enabled);
  if(enabled == false) {
    setWdgValidRecursive(true); // Force all valid
  } else {
    setWdgValidRecursive(false);

    for(auto *w: children) {
      if(w) w->runEventOnChange(true);
    }
  }
}

bool Widget::enabled()
{
  return widget->isEnabled();
}

void Widget::setVisible(bool visible)
{
  if(visible == this->visible()) return;

  QBoxLayout *l = (QBoxLayout *)widget->parentWidget()->layout();

  if(visible == false) {
    if(replwidget == nullptr) delete replwidget;

    replwidget = new QFrame();
    replwidget->setMinimumSize(widget->frameGeometry().size());

    {
      //int l,t,r,b;
      //widget->getContentsMargins(&l,&t,&r,&b);
      //replwidget->setContentsMargins(l, t, r, b);
      QMargins margins = widget->contentsMargins();
      replwidget->setContentsMargins(margins.left(),
                                     margins.top(),
                                     margins.right(),
                                     margins.bottom());
    }

    int idx = l->indexOf(widget);
    l->insertWidget(idx, replwidget);

  } else {
    int idx = l->indexOf(replwidget);
    if(replwidget) {
      delete replwidget;
      replwidget = nullptr;
    }
    l->insertWidget(idx, widget);
  }

  //  replwidget->setVisible(!visible);
  widget->setVisible(visible);

  if(visible) {
    emit eventOnChange();

    for(auto *w: children) {
      if(w) w->runEventOnChange(true);
    }
  }
}

bool Widget::visible()
{
  return widget->isVisible();
}

void Widget::setHidden(bool hidden)
{
  if(hidden == this->hidden()) return;

  widget->setHidden(hidden);
  if(replwidget) replwidget->setHidden(hidden);

  if(macrowindow)
    macrowindow->setFixedHeight(macrowindow->minimumSizeHint().height());

  if(hidden == false) emit eventOnChange();

  for(auto *w: children) {
    if(w) w->runEventOnChange(true);
  }
}

bool Widget::hidden()
{
  return widget->isHidden();
}

void Widget::shortcutActivated()
{
  DEBUG(shortcut, "Shortcut [%s] activated\n", name().toStdString().c_str());
  setKeyboardFocus();
}

bool Widget::setKeyboardFocus()
{
  if((widget->focusPolicy() & Qt::TabFocus) != 0) {
    widget->setFocus();
    return true;
  }

  return false;
}

bool Widget::setKeyboardFocusRecursive()
{
  if(setKeyboardFocus()) return true;

  for(auto *w: children) {
    if(w && w->setKeyboardFocusRecursive()) return true;
  }

  return false;
}

Widget *Widget::findWidget(QString n, bool deep)
{
  DEBUG(widget, "Find Widget '%s' this: '%s' (hide?%d deep?%d #child:%d)\n",
        n.toStdString().c_str(), name().toStdString().c_str(),
        hideChildren, deep, children.size());

  if(n == name()) return this;

  if(hideChildren && deep == false) return nullptr;

  for(auto *w: children) {
    if(w) {
      Widget *wg = w->findWidget(n, deep);
      if(wg) return wg;
    }
  }

  return nullptr;
}

QVector< Widget* > Widget::widgetList(bool deep)
{
  //  DEBUG(widget, "Widget List %p\n", this);

  QVector< Widget* > lst;

  lst.push_back(this);

  if(hideChildren && deep == false) return lst;

  for(auto *w: children) {
    if(w) lst += w->widgetList(deep);
  }

  return lst;
}

void Widget::childWasChanged()
{
  emit wasChanged();
}

void Widget::connectChild(Widget *child)
{
  connect(child, &Widget::wasChanged, this, &Widget::childWasChanged);
}

void Widget::addChild(Widget *widget)
{
  if(widget == nullptr) {
    WARN(widget, "Trying to add nullptr child to '%s'\n",
         name().toStdString().c_str());
    return;
  }
  children.push_back(widget);
  connectChild(widget);
  //widget->setParent(this);
}

void Widget::addChildren(QDomNode &node, QLayout *layout)
{
  QDomNodeList children = node.childNodes();
  for(int i = 0; i < children.count(); i++) {
    QDomNode child = children.at(i);
    createWidget(child, layout);
  }
}

void Widget::setValues()
{
  for(auto *w: children) {
    if(w) w->setValues();
  }

  if(has_lazy) setValue(lazy_value, lazy_source);
  else emit eventOnChange(); // Make sure we run validation on the unset widget.
}

void Widget::setForegroundColour(unsigned char red,
                                 unsigned char green,
                                 unsigned char blue)
{
  char style[128];
  sprintf(style, "* { color: #%02x%02x%02x; }",
          red, green, blue);
  qwidget()->setStyleSheet(style);
}

void Widget::setBackgroundColour(unsigned char red,
                                 unsigned char green,
                                 unsigned char blue)
{
  char style[128];
  sprintf(style, "* { background-color: #%02x%02x%02x; }",
          red, green, blue);
  qwidget()->setStyleSheet(style);
}

void Widget::createWidget(QDomNode &xml_node, QLayout *layout)
{
  QDomElement xml_elem = xml_node.toElement();

  Widget *widget = nullptr;
  if(xml_elem.tagName() == "spacer") {

    if(layout) ((QBoxLayout*)layout)->addStretch();

    return; // This is not a real widget.
  } else if(xml_elem.tagName() == "frame") {

    if(xml_elem.hasAttribute("caption")) {
      GroupBox *frame = new GroupBox(xml_elem, macrowindow);
      widget = frame;
    } else {
      Frame *frame = new Frame(xml_elem, macrowindow);
      widget = frame;
    }

  } else if(xml_elem.tagName() == "checkgroupbox") {

    CheckGroupBox *chkgrpbox = new CheckGroupBox(xml_elem, macrowindow);
    widget = chkgrpbox;

  } else if(xml_elem.tagName() == "label") {

    Label *label = new Label(xml_elem, macrowindow);
    widget = label;

  } else if(xml_elem.tagName() == "lineedit") {

    LineEdit *lineedit = new LineEdit(xml_elem, macrowindow);
    widget = lineedit;

  } else if(xml_elem.tagName() == "datetime") {

    DateTime *datetime = new DateTime(xml_elem, macrowindow);
    widget = datetime;

  } else if(xml_elem.tagName() == "button") {

    Button *button = new Button(xml_elem, macrowindow);
    widget = button;

  } else if(xml_elem.tagName() == "textedit") {

    TextEdit *textedit = new TextEdit(xml_elem, macrowindow);
    widget = textedit;

  } else if(xml_elem.tagName() == "checkbox") {

    CheckBox *checkbox = new CheckBox(xml_elem, macrowindow);
    widget = checkbox;

  } else if(xml_elem.tagName() == "radiobuttons") {

    RadioButtons *radiobuttons = new RadioButtons(xml_elem, macrowindow);
    widget = radiobuttons;

  } else if(xml_elem.tagName() == "combobox") {

    ComboBox *combobox = new ComboBox(xml_elem, macrowindow);
    widget = combobox;

  } else if(xml_elem.tagName() == "listbox") {

    ListBox *listbox = new ListBox(xml_elem, macrowindow);
    widget = listbox;

  } else if(xml_elem.tagName() == "multilist") {

    MultiList *multilist = new MultiList(xml_elem, macrowindow);
    widget = multilist;

  } else if(xml_elem.tagName() == "altcombobox") {

    AltComboBox *altcombobox = new AltComboBox(xml_elem, macrowindow);
    widget = altcombobox;

  } else if(xml_elem.tagName() == "metawidget") {

    MetaWidget *metawidget = new MetaWidget(xml_elem, macrowindow);
    widget = metawidget;

  }

  if(widget) {
    addChild(widget);
    if(widget->qwidget()) {
      if(layout) layout->addWidget(widget->qwidget());
      widget->qwidget()->show();
    }
  }
}

int wdg_name(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  lua_pushstring(L, wdgu->widget->name().toStdString().c_str());

  return 1;
}

int wdg_type(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  // return error code
  lua_pushstring(L, wdgu->widget->type().toStdString().c_str());

  return 1;
}

int wdg_value(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  lua_pushstring(L, wdgu->widget->value().toStdString().c_str());

  return 1;
}

int wdg_set_value(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  const char *val = luaL_checkstring(L, 2);

  wdgu->widget->setValue(val, LUA_SRC);

  return 0;
}

int wdg_readonly(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  lua_pushboolean(L, wdgu->widget->readOnly());

  return 1;
}

int wdg_set_readonly(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  bool val = luaL_checkbool(L, 2);

  wdgu->widget->setReadOnly(val);

  return 0;
}

int wdg_enabled(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  lua_pushboolean(L, wdgu->widget->enabled());

  return 1;
}

int wdg_set_enabled(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  bool val = luaL_checkbool(L, 2);

  wdgu->widget->setEnabled(val);
  
  return 0;
}

int wdg_visible(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  lua_pushboolean(L, wdgu->widget->visible());

  return 1;
}

int wdg_set_visible(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  bool val = luaL_checkbool(L, 2);

  wdgu->widget->setVisible(val);
  
  return 0;
}

int wdg_hidden(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  lua_pushboolean(L, wdgu->widget->hidden());

  return 1;
}

int wdg_set_hidden(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  bool val = luaL_checkbool(L, 2);

  wdgu->widget->setHidden(val);
  
  return 0;
}

int wdg_valid(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  lua_pushboolean(L, wdgu->widget->valid());

  return 1;
}

int wdg_set_valid(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  bool val = luaL_checkbool(L, 2);

  wdgu->widget->setValid(val);
  
  return 0;
}

int wdg_set_bgcol(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  int r = luaL_checknumber(L, 2);
  int g = luaL_checknumber(L, 3);
  int b = luaL_checknumber(L, 4);

  wdgu->widget->setBackgroundColour((unsigned char)r, (unsigned char)g, (unsigned char)b);
  
  return 0;
}

int wdg_set_fgcol(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "widget expected");

  int r = luaL_checknumber(L, 2);
  int g = luaL_checknumber(L, 3);
  int b = luaL_checknumber(L, 4);

  wdgu->widget->setForegroundColour((unsigned char)r, (unsigned char)g, (unsigned char)b);
  
  return 0;
}

