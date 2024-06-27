/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            macrowindow.cc
 *
 *  Fri Aug 31 12:27:45 CEST 2007
 *  Copyright 2007 Lars Bisballe Jensen and Bent Bisballe Nyeng
 *  larsbisballe@gmail.com and deva@aasimon.org
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
#include "macrowindow.h"

#include <QVBoxLayout>
#include <QDomDocument>
#include <QDomElement>
#include <QByteArray>
#include <QApplication>

#include "messagebox.h"
#include "widgets/widget.h"
#include "widgets/window.h"
#include "lua.h"
#include "mainwindow.h"
#include "macrodrawer.h"

#include "debug.h"

extern MainWindow *gmainwindow;

extern QString patientId;
extern QString user;

MacroWindow::MacroWindow(NetCom &n, QString course, QString templ,
                         bool is_static, bool compact,
                         QScrollArea *scrollarea,
                         MacroDrawer *d)
  : Collapser(nullptr, is_static?nullptr:scrollarea), netcom(n)
{
  this->scrollarea = scrollarea;
  drawer = d;
  this->is_static = is_static;

  DEBUG(macrowindow, "Constructor %p\n", this);

  mainwidget = nullptr;
  lua = new LUA(&mainwidget, templ, "", user, patientId);

  waschanged = false;

  this->course = course;
  this->templ = templ;
  
  resumewidget = new ResumeWidget(compact);

  active = true;

  connect(this, &MacroWindow::doneAnimating, this, &MacroWindow::animated);
}

MacroWindow::~MacroWindow()
{
  delete resumewidget;
  /*
  The following line causes segfault if it's included
  clear();
  */
}

void MacroWindow::update(QDomNode &node)
{
  QDomElement elem = node.toElement();
  if(elem.tagName() != "macro") return;

  if(macro == "") macro = elem.attribute("name", "");
  if(version == "") version = elem.attribute("version", "");

  lua->macro = macro;

  if(macro != elem.attribute("name", "")) return;
  if(version != elem.attribute("version", "")) return;

  QDomNodeList children = node.childNodes();

  // No content reveals resumewidget with 'dummy' text.
  if(children.count() == 0) {
    animateToWidget(resumewidget, false);
    return;
  }

  QDomNode childnode = children.at(0);
  QDomElement childelem = childnode.toElement();
  if(childelem.tagName() == "resume") {
    updateResume(childnode);
    return;
  }
  
  initMacro(node);
}

void MacroWindow::updateResume(QDomNode &node)
{
  QDomElement elem = node.toElement();

  if(elem.tagName() != "resume") return;

  QString resume = elem.text();
  ResumeWidget::state_t state = ResumeWidget::OLD;
  if(elem.hasAttribute("state")) {
    if(elem.attribute("state") == "old") state = ResumeWidget::OLD;
    if(elem.attribute("state") == "new") state = ResumeWidget::NEW;
    if(elem.attribute("state") == "dirty") state = ResumeWidget::DIRTY;
  }

  resumewidget->setText(resume, state);
  animateToWidget(resumewidget, false);
}

void MacroWindow::initMacro(QDomNode &node)
{
  QDomElement elem = node.toElement();

  if(elem.tagName() == "scripts") {
    // Nothing to do here
  }

  if(elem.tagName() == "script") {
    lua->runScript(elem.text(), nullptr, "preload");
  }

  if(elem.tagName() == "widgets") {
    Window *window = new Window(elem, this);
    mainwidget = window;
    connect(window, &Window::wasChanged, this, &MacroWindow::macroChanged);
    macrotitle = elem.attribute("caption");
    window->setValues();
    window->runEventOnInit(true);

    if(waschanged == true) macroChanged();

    WARN(macrowindow, "New window.\n");
    animateToWidget(mainwidget->qwidget(), true);

    return; // No further recursion here.
  }

  QDomNodeList children = node.childNodes();

  for (int i=0; i<children.count();i++) {
    QDomNode child = children.at(i);
    initMacro(child);
  }
}

bool MacroWindow::doCommit()
{
  if(mainwidget->committable(true) == false) return false;

  if(mainwidget->valid()) {

    QVector< Widget* > wlist;
    if(mainwidget) wlist = mainwidget->widgetList();

    // Disable/grey commit button when committing. We have to disable it here
    //   in order to accommodate 'multilist' widgets with unsaved data.
    for(auto *w: wlist) {
      if(w->objectName() == "commitButton") {
        emit setBusyTitle();
        w->setEnabled(false);
      }
    }
    
    netcom.send(wlist, course, templ, macro, version);
    return true;

  } else {
    MessageBox::critical(nullptr, tr("Error"),
                         tr("The macro ") + macrotitle +
                         tr(" was not filled out correctly, please try again.\n"),
                         MessageBox::Ok);

    return false;
  }
}

void MacroWindow::commit()
{
  doCommit();
  drawer->setFocus();
}

void MacroWindow::cancel()
{
  collapseWrapper();
  drawer->setFocus();
}

void MacroWindow::expandWrapper()
{
  if(currentWidget() != resumewidget) return;
  netcom.send(course, templ, macro);
}

void MacroWindow::collapseWrapper()
{
  if(currentWidget() == resumewidget) return;

  if(waschanged == false) {
    netcom.send(course, templ);
    return;
  }

  MessageBox::StandardButton res =
    MessageBox::warning(nullptr,
                        tr("Save the macro changes?"),
                        tr("you have choosen to close the macro ") +
                        macrotitle + ".\n" +
                        tr("do you want to save before closing?"),
                        MessageBox::Save | MessageBox::Close |
                        MessageBox::Cancel);

  switch(res) {
  case MessageBox::Save:
    doCommit();
    break;
  case MessageBox::Close:
    netcom.send(course, templ);
    break;
  case MessageBox::Cancel:
    emit revertTitle();
    break;
  default:
    break;
  }
}

void MacroWindow::toggleMacro()
{
  if(!active) {
    emit revertTitle();
    return;
  }

  if(currentWidget() == resumewidget) {
    if(gmainwindow && !gmainwindow->hasOpen(this)) {
      expandWrapper();
    } else {
      MessageBox::warning(this, tr("Close first"),
                          tr("Close other one first."));
      emit revertTitle();
    }
  } else {
    collapseWrapper();
  }
}

void MacroWindow::macroChanged()
{
  DEBUG(macrowindow, "This macro was changed!\n");
  emit macroHasChanged();
  waschanged = true;
}

void MacroWindow::setActive(bool active)
{
  if(this->active == active) return;

  this->active = active;
  setEnabled(active);

  emit activationChanged(active);
}

void MacroWindow::clear()
{
  lua->clear();

  if(mainwidget) {
    drawer->setFocus();
    mainwidget->deleteLater();
    mainwidget = nullptr;
  }
}

QWidgetList getRecursiveWidgetList(QWidget *w)
{
  QWidgetList list;

  list.append(w);

  QObjectList objs = w->children();
  for(int i = 0; i < objs.size(); i++) {
    if(objs[i]->isWidgetType()) {
      QWidget *cw = (QWidget*)objs[i];
      list.append(getRecursiveWidgetList(cw));
    }
  }

  return list;
}

void setTabOrderRecursive(QWidget *w)
{
  QWidgetList list = getRecursiveWidgetList(w);
  QWidget *last = nullptr;
  for(int i = 0; i < list.size(); i++) {
    QWidget *w = list[i];
    if((w->focusPolicy() & Qt::TabFocus) != 0) {
      if(last) QWidget::setTabOrder(last, w);
      last = w;
    }
  }
}

void MacroWindow::animated(QWidget *w)
{
  if(w == resumewidget) {

    clear();

  } else {

    if(mainwidget == nullptr) return;

    waschanged = false;
    mainwidget->setValues();

    if(is_static) return;

    setTabOrderRecursive(scrollarea);

    // Set keyboard focus on the first focusable widget in the macro.
    mainwidget->setKeyboardFocusRecursive();
  }
}
