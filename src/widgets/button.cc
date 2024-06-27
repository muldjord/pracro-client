/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            button.cc
 *
 *  Fri Jul 13 12:38:45 CEST 2007
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
#include "button.h"

#include <stdio.h>
#include <QPushButton>

#include "common.h"
#include "debug.h"

#include "macrowindow.h"

Button::Button(QDomNode &node, MacroWindow *macrowindow)
  : Widget(node, macrowindow)
{
  button = new QPushButton();
  setWidget(button);

  setCommonAttributes(button, node);

  QDomElement elem = node.toElement();
  
  button->setText(elem.attribute("caption", ""));

  if(elem.hasAttribute("action")) {
    if(elem.attribute("action") == "commit") {
      setObjectName("commitButton");
      connect(button, &QPushButton::clicked, this, &Button::commit);
      button->setIcon(QPixmap(":add.png"));
      //button->setEnabled(false);
    } else if(elem.attribute("action") == "cancel") {
      connect(button, &QPushButton::clicked, this, &Button::cancel);
      button->setIcon(QPixmap(":del.png"));
    }
  }

  if(macrowindow) {
    connect(this, &Button::act_commit, macrowindow, &MacroWindow::commit);
    connect(this, &Button::act_cancel, macrowindow, &MacroWindow::cancel);
    connect(macrowindow, &MacroWindow::macroHasChanged, this, &Button::do_enable);
  }

  if(elem.attribute("action") == "commit") {
    QList<QKeySequence> lst;
    lst << QKeySequence("Ctrl+Enter");
    lst << QKeySequence("Ctrl+Return");
    QAction *a = new QAction(widget_name, this);
    a->setShortcuts(lst);
    button->addAction(a);
    connect(a, &QAction::triggered, this, &Button::shortcutActivated);
  }
}

Button::~Button()
{
  //  delete button;
}

void Button::commit()
{
  emit act_commit();
}

void Button::cancel()
{
  emit act_cancel();
}

void Button::do_enable()
{
  button->setEnabled(true);
}

void Button::shortcutActivated()
{
  setKeyboardFocus();
  button->click();
}
