/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            checkgroupbox.h
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
#ifndef __PRACRO_CHECKGROUPBOX_H__
#define __PRACRO_CHECKGROUPBOX_H__

#include "checkbox.h"

#include <QDomNode>
#include <QCheckBox>

#ifdef LUA_CLASS
#undef LUA_CLASS
#endif
#define LUA_CLASS "CheckGroupBox"

/***
 * Checkable GroupBox Widget
 * @tag checkgroupbox
 * @clientside
 * @extends checkbox
 * @screenshot Example 1: type="framed"
 * <checkgroupbox name="example" caption="Example" type="framed" layout="vbox"
 *                truevalue="true" falsevalue="false" value="true">
 *   <lineedit name="dims" value="Contained widget 1"/>
 *   <lineedit name="dims" value="Contained widget 2"/>
 * </checkgroupbox>
 * @screenshot Example 2: type="simple"
 * <checkgroupbox name="example" caption="Example" type="simple" layout="vbox"
 *                truevalue="true" falsevalue="false" value="true">
 *   <lineedit name="dims" value="Contained widget 1"/>
 *   <lineedit name="dims" value="Contained widget 2"/>
 * </checkgroupbox>
 * @container 
 * @att layout The layout used in the groupbox. Can be one of 'vbox' or 'hbox'.
 * @att type Defines the type of the checkbox. It can be one of 'framed' or
 * 'simple'. Framed will draw a frame with the checkbox contained in the
 * caption. 'simple' will draw the checkbox without the frame and put the inner
 * to the right of it. Default is 'framed'.
 */

class CheckGroupBox : public CheckBox
{
Q_OBJECT
public:
  CheckGroupBox(QDomNode &node, MacroWindow *macrowindow);
  ~CheckGroupBox();

  //void setWdgValid(bool valid);

  bool setKeyboardFocus();

public slots:
  void cgb_state_change();

private:
  QString type;
  bool changedByUser;

  QCheckBox *checkbox;
};

const struct luaL_Reg chkgrpbox_meths[] =
  { WDG_METHS, CHKBOX_METHS, {nullptr, nullptr} };

inline void register_checkgroupbox(lua_State *L)
{
  register_widget(L);

  luaL_newmetatable(L, LUA_CLASS);
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);
  lua_rawset(L, -3);
  luaL_register(L, nullptr, chkgrpbox_meths);
}

#endif/*__PRACRO_CHECKGROUPBOX_H__*/
