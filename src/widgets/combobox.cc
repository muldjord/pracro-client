/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            combobox.cc
 *
 *  Wed Jul 18 10:35:52 CEST 2007
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
#include "combobox.h"

#include <QComboBox>

#include <QDomNodeList>
#include <QCompleter>
#include <QRegExpValidator>
#include <QRegExp>
#include <QLineEdit>
#include <QCoreApplication>

#include <QEvent>
#include <QKeyEvent>
#include <QWheelEvent>

#include "common.h"
#include "debug.h"
#include "luawidget.h"

// Enable this to make the combobox drawn in windows style.
// This will make its background red even when not expanded.
//#define STYLE_HACK

#ifdef STYLE_HACK
#include <QWindowsStyle>
QWindowsStyle s;
#endif/*STYLE_HACK*/

class MyQComboBox : public QComboBox {
protected:
  void wheelEvent(QWheelEvent *e)
  {
    QCoreApplication::sendEvent(nativeParentWidget(), e);
  }
};

ComboBox::ComboBox(QDomNode &node, MacroWindow *macrowindow)
  : Widget(node, macrowindow)
{
  combobox = new MyQComboBox();
  setWidget(combobox);

  ignoreChangeEvents = false;

  setCommonAttributes(combobox, node);

#ifdef STYLE_HACK
  combobox->setStyle(&s);
#endif/*STYLE_HACK*/

  combobox->setInsertPolicy(QComboBox::InsertAlphabetically);

  QDomNodeList children = node.childNodes();
  //  QStringList itemlist;

  for (int i=0; i<children.count();i++) {
    QDomNode child = children.at(i);
    QDomElement combo_elem = child.toElement();
    if(combo_elem.hasAttribute("caption") && combo_elem.hasAttribute("value")) {
      combobox->addItem(combo_elem.attribute("caption"),
                        combo_elem.attribute("value"));
      itemlist << combo_elem.attribute("caption");
    } else {
      printf("XML Error!!! Combobox is missing one or more attributes...\n");
    }
  }

  // Make empty default selection.
  combobox->setCurrentIndex(-1);

  combobox->installEventFilter(this);

  QDomElement elem = node.toElement();

  combotype = SELECT;
  if(elem.hasAttribute("type")) {
    if(elem.attribute("type") == "select") combotype = SELECT;
    if(elem.attribute("type") == "edit") combotype = EDIT;
    if(elem.attribute("type") == "search") combotype = SEARCH;
  }

  switch(combotype) {
  case SELECT:
    {
      combobox->setEditable(false);

#ifndef STYLE_HACK
      combobox->setEditable(true);
      combobox->lineEdit()->setReadOnly(true);
      combobox->lineEdit()->installEventFilter(this);
#endif/*STYLE_HACK*/

      connect(combobox, qOverload<int>(&MyQComboBox::currentIndexChanged), this, &ComboBox::changed);
    }
    break;

  case EDIT:
    {
      combobox->setEditable(true);
      connect(combobox, &MyQComboBox::editTextChanged, this, &ComboBox::changed);
      QCompleter *completer = combobox->completer();
      completer->setCaseSensitivity(Qt::CaseInsensitive);
      completer->setCompletionMode(QCompleter::PopupCompletion);
    }
    break;

  case SEARCH:
    combobox->setEditable(true);

    updateSearchList();

    connect(combobox, &MyQComboBox::editTextChanged, this, &ComboBox::changed);
    break;
  }
}

ComboBox::~ComboBox()
{
  // delete combobox;
}

void ComboBox::updateSearchList()
{
  {
    QString rxs = "(";
    for(int i = 0; i < itemlist.size(); i++) {
      if(rxs != "(") rxs += "|";
      rxs += QRegExp::escape(itemlist.at(i));
    }
    rxs += ")";
    rx = QRegExp(rxs);
    rx.setCaseSensitivity(Qt::CaseInsensitive);
  }
  {    
    QCompleter *completer = new QCompleter(itemlist, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    combobox->setCompleter(completer);
    combobox->setValidator(new QRegExpValidator(rx, this));
  }
}

QString ComboBox::value()
{
  QString value;

  int idx = combobox->findText(combobox->currentText());
  /*
  DEBUG(combobox, "idx => %d currentText => '%s'\n", idx,
        combobox->currentText().toStdString().c_str());
  */
  if(idx != -1) value = combobox->itemData(idx).toString();
  else value = combobox->currentText();

  DEBUG(combobox, "value() => '%s'\n", value.toStdString().c_str());

  return value;
}

void ComboBox::setValue(QString value, QString source)
{
  if(isUserSource(source)) emit wasChanged();

  int idx = combobox->findData(value);

  if(combobox->currentIndex() != idx) {
    ischangingbyuser = false;
    combobox->setCurrentIndex(idx);
    ischangingbyuser = true;
  } else {
    emit eventOnChange();
  }
}

bool ComboBox::preValid()
{
  if(combotype == SELECT) {
    if(combobox->currentIndex() != -1) return true;
    else return false;
  }
  return true;
}

void ComboBox::changed()
{
  if(ignoreChangeEvents == true) return;
  if(ischangingbyuser) emit wasChanged();
  emit eventOnChange();
}

bool ComboBox::eventFilter(QObject *obj, QEvent *event)
{
  if(ignoreChangeEvents == true) return Widget::eventFilter(obj, event);

  if(combotype == SELECT && obj == combobox->lineEdit()) {
    if(event->type() == QEvent::MouseButtonRelease) {
      if(enabled()) combobox->showPopup();
    }
  }

  if(event->type() == QEvent::KeyPress) {
    QKeyEvent *ke = (QKeyEvent*)event;
    //    printf("KEY: %d\n", ke->key());
    //    if(ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down) {
    if(ke->key() == Qt::Key_Space) {
      if(enabled()) combobox->showPopup();
    }
  }

  return Widget::eventFilter(obj, event);
}

void ComboBox::changeEvent(QEvent *event)
{
  if(event->type() == QEvent::EnabledChange) changed();
}

void ComboBox::setWdgValid(bool valid)
{
  QPalette palette;

  if(valid) {
    // valid string
    palette.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255)));
  } else {
    // invalid string
    palette.setBrush(QPalette::Base, QBrush(QColor(230, 200, 200)));
  }

  if(combobox->lineEdit()) combobox->lineEdit()->setPalette(palette);
  combobox->setPalette(palette);
}

void ComboBox::clear()
{
  ignoreChangeEvents = true;
  combobox->clear();
  ignoreChangeEvents = false;
}

void ComboBox::addItem(QString item, QString value)
{
  ignoreChangeEvents = true;
  combobox->addItem(item, value);

  if(combotype == EDIT) {
    combobox->completer()->complete();
  }

  if(combotype == SEARCH) {
    itemlist.push_back(item);
    updateSearchList();
  }

  ignoreChangeEvents = false;
}

QString ComboBox::lineEditValue()
{
  if(combobox->lineEdit()) return combobox->lineEdit()->text();
  return "";
  
}

void ComboBox::setLineEditValue(QString value)
{
  ignoreChangeEvents = true;
  if(combobox->lineEdit()) {
    combobox->lineEdit()->setText(value);
  }
  ignoreChangeEvents = false;
}

int cmb_clear(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "combobox expected");

  ComboBox *cmb = (ComboBox*)wdgu->widget;
  cmb->clear();

  return 0;
}

int cmb_add_item(lua_State *L)
{
  wdg_userdata *wdgu;

  size_t nargs = lua_gettop(L); // number of arguments

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "combobox expected");

  QString cap = luaL_checkstring(L, 2);
  QString val = cap;
  if(nargs == 3) {
    val = luaL_checkstring(L, 3);
  }

  ComboBox *cmb = (ComboBox*)wdgu->widget;
  cmb->addItem(cap, val);

  return 0;
}

int cmb_le_value(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "combobox expected");

  ComboBox *cmb = (ComboBox*)wdgu->widget;
  lua_pushstring(L, cmb->lineEditValue().toStdString().c_str());

  return 1;
}

int cmb_le_set_value(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "combobox expected");

  const char *val = luaL_checkstring(L, 2);

  ComboBox *cmb = (ComboBox*)wdgu->widget;
  cmb->setLineEditValue(val);

  return 0;
}
