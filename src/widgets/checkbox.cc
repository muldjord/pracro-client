/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            checkbox.cc
 *
 *  Wed Jul 18 10:45:29 CEST 2007
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
#include "checkbox.h"

#include <QCheckBox>

#include "debug.h"

#include "common.h"
#include "luawidget.h"

CheckBox::CheckBox(QDomNode &node, MacroWindow *macrowindow)
  : Widget(node, macrowindow)
{
  checkbox = new QCheckBox();
  setWidget(checkbox);

  changedByUser = true;
  
  setCommonAttributes(checkbox, node);

  QDomElement elem = node.toElement();

  if(elem.hasAttribute("caption")) {
    checkbox->setText(elem.attribute("caption"));
  }

  if(elem.hasAttribute("truevalue")) {
    truevalue = elem.attribute("truevalue");
  } else {
    truevalue = "true";
  }

  if(elem.hasAttribute("falsevalue")) {
    falsevalue = elem.attribute("falsevalue");
  } else {
    falsevalue = "false";
  }
  
  connect(checkbox, &QCheckBox::stateChanged, this, &CheckBox::state_change);
}

CheckBox::~CheckBox()
{
  //  delete checkbox;
}

QString CheckBox::value()
{
  if(checkbox->isChecked()) return truevalue;
  return falsevalue;
}

void CheckBox::setValue(QString value, QString source)
{
  if(isUserSource(source)) emit wasChanged();

  changedByUser = false;

  bool old = checkbox->isChecked();

  if(value == truevalue) {
    checkbox->setChecked(true);
  } else if(value == falsevalue) {
    checkbox->setChecked(false);
  } else {
    printf("Unknown checkbox value: %s\n", value.toStdString().c_str());
  }

  // If set operation did not change the value we must invocate the code manually.
  if(old == checkbox->isChecked()) state_change(0);

  //  setInitialValue(value);

  changedByUser = true;
}

void CheckBox::state_change(int)
{
  emit eventOnChange();
  if(changedByUser) emit wasChanged();
}

bool CheckBox::checked()
{
  return value() == truevalue;
}

void CheckBox::setChecked(bool checked)
{
  setValue(checked ? truevalue : falsevalue);
}

void CheckBox::setWdgValid(bool valid)
{
  QPalette palette;

  if(valid) {
    // valid string
    palette.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255)));
  } else {
    // invalid string
    palette.setBrush(QPalette::Base, QBrush(QColor(230, 200, 200)));
  }

  checkbox->setPalette(palette);
}

void CheckBox::shortcutActivated()
{
  setKeyboardFocus();
  checkbox->click();//setChecked(!checked());
}

int chk_checked(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "checkbox expected");

  CheckBox *chk = (CheckBox*)wdgu->widget;
  lua_pushboolean(L, chk->checked());

  return 1;
}

int chk_set_checked(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "checkbox expected");

  bool val = luaL_checkbool(L, 2);

  CheckBox *chk = (CheckBox*)wdgu->widget;
  chk->setChecked(val);
  
  return 0;
}
