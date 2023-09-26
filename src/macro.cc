/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            macro.cc
 *
 *  Tue Jun 30 11:49:46 CEST 2009
 *  Copyright 2009 Bent Bisballe Nyeng
 *  deva@aasimon.org
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
#include "macro.h"

#include <QDomElement>
#include <QHBoxLayout>
#include <QApplication>
#include <QLabel>

#include "macrodrawer.h"
#include "macrowindow.h"

Macro::Macro(Entities &e, NetCom &n, QString c, QString t, QScrollArea *s)
  : entities(e), netcom(n), course(c), templ(t)
{
  window = nullptr;
  drawer = nullptr;

  scrollarea = s;

  isstatic = false;
  iscompact = false;
  isimportant = false;
}

const char *Macro::type()
{
  return "macro";
}

void Macro::update(QDomNode &node)
{
  QDomElement elem = node.toElement();

  if(elem.tagName() != "macro") return;

  name = elem.attribute("name");
  iscompleted = elem.attribute("completed", "false") == "true";
  isstatic = elem.attribute("static", "false") == "true";
  iscompact = elem.attribute("compact", "false") == "true";
  isimportant = elem.attribute("important", "false") == "true";

  if(drawer == nullptr) {
    drawer = new MacroDrawer(this, elem.attribute("caption", name));

    if(window == nullptr) {
      window = new MacroWindow(netcom, course, templ, isstatic, iscompact,
                               scrollarea, drawer);
      
      QFont f = window->font();
      f.setBold(false);
      f.setItalic(false);
      window->setFont(f);
    }

    QHBoxLayout *l = new QHBoxLayout();
    l->setContentsMargins(10,0,10,0);
    drawer->setLayout(l);
    l->addWidget(window);

    QObject::connect(drawer, &MacroDrawer::toggle, window, &MacroWindow::toggleMacro);
    QObject::connect(window, &MacroWindow::activationChanged,
                     drawer, &MacroDrawer::activationChange);
    QObject::connect(window, &MacroWindow::setBusyTitle, drawer, &MacroDrawer::setBusyTitle);
    QObject::connect(window, &MacroWindow::revertTitle, drawer, &MacroDrawer::revertTitle);
  }

  window->update(node);

  window->setActive(false);

  // Read and parse requirement list.
  if(elem.hasAttribute("requires")) {
    QString req = elem.attribute("requires");
    requires = req.split(QRegExp("\\W+"), Qt::SkipEmptyParts);
  }

  // Test if requirements are fulfilled
  QStringList::iterator rs = requires.begin();
  QStringList blocking;
  bool active = true;
  while(rs != requires.end()) {
    Entities::iterator es = entities.begin();
    while(es != entities.end()) {
      Entity *e = *es;
      QString type = e->type();
      if(type == "macro") {
        Macro *m = (Macro*)e;
        if(m->name == *rs) {
          if(m->iscompleted == false) blocking.append(m->name);
          active = active && m->iscompleted;
        }
      }
      es++;
    }
    rs++;
  }
  window->setActive(active);
  if(!active) drawer->setToolTip(QObject::tr("Depends on: ") +
                                 blocking.join(", "));
  else drawer->setToolTip("");
}

QWidget *Macro::widget()
{
  return drawer;
}

bool Macro::isOpen()
{
  return window && isstatic == false &&
    window->currentWidget() != window->resumewidget;
      
}
