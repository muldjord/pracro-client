/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            checkgroupbox.cc
 *
 *  Thu Mar 10 10:51:10 CET 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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
#include "checkgroupbox.h"

#include <QGroupBox>
#include <QHBoxLayout>

#include "common.h"
#include "debug.h"

CheckGroupBox::CheckGroupBox(QDomNode &node, MacroWindow *macrowindow)
  : CheckBox(node, macrowindow)
{
  checkbox = (QCheckBox*)getWidget();
  connect(this, &CheckGroupBox::wasChanged, this, &CheckGroupBox::cgb_state_change);

  QDomElement elem = node.toElement();
  type = elem.attribute("type", "framed");

  if(type == "framed") {
    QGroupBox *gb = new QGroupBox();
    gb->setTitle(" ");
    setWidget(gb);
  } else if(type == "simple") {
    setWidget(new QWidget());
  } else {
    ERR(checkgroupbox, "Illegal value of attribute 'type'\n");
    return;
  }

  setCommonLayout(getWidget(), node);
  setCommonAttributes(getWidget(), node);

  checkbox->setParent(getWidget());
  checkbox->resize(checkbox->sizeHint().width(), 32);
  checkbox->show();

  if(type == "framed") {
    getWidget()->setContentsMargins(0, 10, 0, 0);
    checkbox->move(5, -9);
    checkbox->setAutoFillBackground(true);
  }

  if(type == "simple") {
    getWidget()->setContentsMargins(checkbox->sizeHint().width(), 0, 0, 0);
    checkbox->move(0, 3);
  }

  addChildren(node, getWidget()->layout());

  // Hack to ensure all widgets inside group are enabled as expected.
  QTimer::singleShot(100, this, SLOT(cgb_state_change()));
}

CheckGroupBox::~CheckGroupBox()
{
}

void CheckGroupBox::cgb_state_change()
{
  QVector< Widget* >::iterator i = children.begin();
  while(i != children.end()) {
    Widget *w = *i;
    if(w) w->setEnabled(checkbox->isChecked());
    i++;
  }
}

bool CheckGroupBox::setKeyboardFocus()
{
  checkbox->setFocus();
  return true;
}
