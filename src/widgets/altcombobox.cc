/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            altcombobox.cc
 *
 *  Tue Nov 25 08:18:10 CET 2008
 *  Copyright 2008 Bent Bisballe Nyeng
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
#include "altcombobox.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLineEdit>

#include "common.h"
#include "multilist.h"
#include "macrowindow.h"

#include "debug.h"
#include "luawidget.h"

AltComboBox::AltComboBox(QDomNode &node, MacroWindow *macrowindow)
  : ComboBox(node, macrowindow)
{
  combobox = (QComboBox*)getWidget();
  frame = new QFrame();
  setWidget(frame);

  hideChildren = true;

  innerwidget = nullptr;

  setCommonAttributes(frame, node);
  setCommonLayout(frame, node);
  
  frame->layout()->addWidget(combobox);
  frame->layout()->setContentsMargins(0,0,0,0);

  altframe = nullptr;

  QString iwname;

  QDomNodeList items = node.childNodes();
  for(int i = 0; i < items.count(); i++) {
    QDomElement item = items.at(i).toElement();

    if(item.tagName() == "altitem") {
      altframe = new Frame(item, macrowindow);

      if(item.hasAttribute("value")) {
        altvalue = item.attribute("value");
      } else {
        ERR(altcombobox, "altitem tag is missing the value attribute, "
              "in altcombobox!\n");
      }

      if(item.hasAttribute("innerwidget")) {
        iwname = item.attribute("innerwidget");
      } else {
        ERR(altcombobox, "altitem tag is missing the innerwidget attribute, "
               "in altcombobox!\n");
      }

      frame->layout()->addWidget(altframe->qwidget());
      children.push_back(altframe);

      break;
    }
  }

  innerwidget = findWidget(iwname, true);

  if(innerwidget == nullptr) {
    ERR(altcombobox, "Inner Widget %s not found (in altcombobox)!\n",
          iwname.toStdString().c_str());
  }

  // Set visibility of altframe when combo selection changes.
  connect(this, &AltComboBox::eventOnChange, this, &AltComboBox::comboChanged);

  // Propagate wasChanged events in the innerwidget
  if(innerwidget)
    connect(innerwidget, &Widget::wasChanged, this, &AltComboBox::onChildChange);
}

AltComboBox::~AltComboBox()
{
}

bool AltComboBox::setKeyboardFocus()
{
  combobox->setFocus();
  return true;
}

QComboBox *AltComboBox::qcombobox()
{
  return combobox;
}

bool AltComboBox::preValid()
{
  if(ComboBox::preValid() == false) return false;

  if(innerwidget && ComboBox::value() == altvalue) {
    if(!innerwidget->valid()) return false;
  }

  return true;
}

QString AltComboBox::value()
{
  QString val = ComboBox::value();

  DEBUG(altcombobox, "ComboBox::value() => %s\n", val.toStdString().c_str());

  if(val == altvalue) {
    if(innerwidget) return innerwidget->value();
    else return "";
  } else {
    return val;
  }
}

void AltComboBox::setValue(QString value, QString source)
{
  QComboBox *cmb = combobox;
  if(cmb->findData(value) != -1) {
    ComboBox::setValue(value, source);
  } else {
    ComboBox::setValue(altvalue);

    if(innerwidget) {
      innerwidget->setValue(value, source);
    }
  }
}

void AltComboBox::comboChanged()
{
  DEBUG(altcombobox, "comboChanged to '%s'\n",
        ComboBox::value().toStdString().c_str());

  if(altframe) {
    DEBUG(altcombobox, "inner->setVisible(%d)\n",
          ComboBox::value() == altvalue);
    altframe->setVisible(ComboBox::value() == altvalue);
  }
}

void AltComboBox::onChildChange()
{
  DEBUG(altcombobox, "onChildChange\n");
  if(ComboBox::value() == altvalue) {
    emit eventOnChange(true);
    emit wasChanged();
  }
}
