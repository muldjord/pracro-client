/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            checkbox.h
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
#ifndef __PRACRO_CHECKBOX_H__
#define __PRACRO_CHECKBOX_H__

#include "widget.h"
#include <QDomNode>

#ifdef LUA_CLASS
#undef LUA_CLASS
#endif
#define LUA_CLASS "CheckBox"

/***
 * CheckBox Widget
 * @tag checkbox
 * @clientside
 * @screenshot An example checkbox
 * <checkbox name="example" truevalue="true" falsevalue="false" value="true"
 *           caption="A simple checkbox"/>
 * @extends widget
 * This widget is used to retrieve boolean values from the user. The widget can
 * either be in checked state or unschecked state. The value of the widget
 * depends on its state and the values of the falseval and trueval attributes.
 * @att caption The caption of the checkbox.
 * @att trueval The value of the widget if it is checked.
 * @att falseval The value of the widget if it is unchecked.
 */

class QCheckBox;
class CheckBox : public Widget
{
Q_OBJECT
public:
  CheckBox(QDomNode &node, MacroWindow *macrowindow);
  ~CheckBox();

  virtual QString luaclass() { return LUA_CLASS; }

  QString value();
  void setValue(QString value, QString source = "");

  void setWdgValid(bool valid);

  bool checked();
  void setChecked(bool checked);

public slots:
  void state_change(int);
  void shortcutActivated();


private:
  QString truevalue;
  QString falsevalue;

  bool changedByUser;
  
  QCheckBox *checkbox;
};

/***
 * @method boolean checked()
 * This method retrives the current check state of the checkbox.
 * @return the boolean value true if the checkbox is checked. Return
 * false if not.
 */
int chk_checked(lua_State *L);

/***
 * @method nil setChecked(boolean checked)
 * This method sets the current check state of the checkbox.
 * @param checked A boolean value. If the value is true the checkbox is set to
 * the state 'checked', otherwise it is set to the state 'unchecked'.
 */
int chk_set_checked(lua_State *L);

#define CHKBOX_METHS \
  {"checked", chk_checked},\
  {"setChecked", chk_set_checked}

const struct luaL_Reg chkbox_meths[] =
  { WDG_METHS, CHKBOX_METHS, {NULL, NULL} };

inline void register_checkbox(lua_State *L)
{
  register_widget(L);

  luaL_newmetatable(L, LUA_CLASS);
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);
  lua_rawset(L, -3);
  luaL_register(L, NULL, chkbox_meths);
}

#endif/*__PRACRO_CHECKBOX_H__*/
