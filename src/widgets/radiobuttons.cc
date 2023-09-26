/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            radiobuttons.cc
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
#include "radiobuttons.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>

#include "common.h"
#include "radiobutton.h"

RadioButtons::RadioButtons(QDomNode &node, MacroWindow *macrowindow)
  : Widget(node, macrowindow)
{
  frame = new QFrame();
  setWidget(frame);

  setCommonAttributes(frame, node);
  setCommonLayout(frame, node);

  QDomNodeList children = node.childNodes();

  for (int i=0; i<children.count();i++) {
    QDomNode child = children.at(i);
    QDomElement radio_elem = child.toElement();
    RadioButton *radiobutton = new RadioButton(child);

    // Create radiobutton from child, insert in this
    frame->layout()->addWidget(radiobutton);

    connect(radiobutton, &RadioButton::clicked, this, &RadioButtons::childChanged);

    radiobutton_list.push_back(radiobutton);
  }
  
  frame->layout()->setContentsMargins(0,0,0,0);
}

RadioButtons::~RadioButtons()
{
  // delete frame;
}

void RadioButtons::setWdgValid(bool valid)
{
  QPalette palette;

  if(valid) {
    palette.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255)));
  } else {
    //palette.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255)));
    palette.setBrush(QPalette::Base, QBrush(QColor(230, 200, 200)));
  }

  QVector< RadioButton* >::iterator i;
  for (i = radiobutton_list.begin(); i != radiobutton_list.end(); ++i) {
    RadioButton *widget = *i;
    widget->setPalette(palette);
  }
}

bool RadioButtons::preValid()
{
  QVector< RadioButton* >::iterator i;
  for (i = radiobutton_list.begin(); i != radiobutton_list.end(); ++i) {
    RadioButton *rbtn = *i;
    if(rbtn->isChecked()) {
      return true;
    }
  }
  return false;
}

QString RadioButtons::value()
{
  QVector< RadioButton* >::iterator i;
  QString value;
  for (i = radiobutton_list.begin(); i != radiobutton_list.end(); ++i) {
    RadioButton *rbtn = *i;
    if(rbtn->isChecked()) {
      /*
      printf("Radiobutton returned: %s\n",
             rbtn->getValue().toStdString().c_str());
      */
      value = rbtn->getValue();
    }
  }
  return value;
}

void RadioButtons::setValue(QString value, QString source)
{
  if(isUserSource(source)) emit wasChanged();

  QVector< RadioButton* >::iterator i;
  for (i = radiobutton_list.begin(); i != radiobutton_list.end(); ++i) {
    RadioButton *rbtn = *i;
    if(value == rbtn->getValue()) {
      rbtn->setChecked(true);
    } else {
      rbtn->setChecked(false);
    }
  }

  //  setInitialValue(value);
}

bool RadioButtons::setKeyboardFocus()
{
  QVector< RadioButton* >::iterator i = radiobutton_list.begin();
  while(i != radiobutton_list.end()) {
    RadioButton *rb = *i;
    if(rb->isChecked()) {
      rb->setFocus();
      return true;
    }
    i++;
  }

  if(radiobutton_list.size()) {
    radiobutton_list.at(0)->setFocus();
    return true;
  }

  return false;
}

void RadioButtons::childChanged()
{
  emit eventOnChange();
  emit wasChanged();
}

void RadioButtons::changeEvent(QEvent *event)
{
  if(event->type() == QEvent::EnabledChange) {
    //    setWdgValid(valid() || !frame->isEnabled());
  }
}
