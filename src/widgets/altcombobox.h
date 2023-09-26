/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            altcombobox.h
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
#ifndef __PRACRO_ALTCOMBOBOX_H__
#define __PRACRO_ALTCOMBOBOX_H__

#include "widget.h"

#include "combobox.h"
#include "frame.h"

#include <QDomNode>
#include <QMap>

#ifdef LUA_CLASS
#undef LUA_CLASS
#endif
#define LUA_CLASS "AltComboBox"

/***
 * ComboBox Widget with Alternate Value
 * @tag altcombobox
 * @clientside
 * @extends combobox
 * @screenshot Example: altitem selected.
 * <altcombobox name="ex" layout="vbox" value="altitem" type="select">
 *   <item caption="Item" value="item"/>
 *   <altitem caption="Alt Item" value="altitem" innerwidget="in" layout="hbox">
 *     <label caption="Alt label:"/>
 *     <lineedit name="in" value="Alt value"/>
 *   </altitem>
 * </altcombobox>
 * @screenshot Example: altitem not selected.
 * <altcombobox name="ex" layout="vbox" value="item" type="select">
 *   <item caption="Item" value="item"/>
 *   <altitem caption="Alt Item" value="altitem" innerwidget="in" layout="hbox">
 *     <label caption="Alt label:"/>
 *     <lineedit name="in" value="Alt value"/>
 *   </altitem>
 * </altcombobox>
 * @container
 * The AltComboBox is used to make a normal selection with a ComboBox but with
 * a special list item that shows an alternate widget and uses this widget for
 * input. The AltComboBox contains <code>&lt;item&gt;</code> tags in the same
 * way as the ComboBox, but may also contain an <code>&lt;altitem&gt;</code> tag
 * which can again contain widgets.
 * @att value [altitem] The value of the item. This will be the value of the
 * AltComboBox if this item is selected.
 * @att caption [altitem] The caption of this item. This is the text presented
 * to the user in the ComboBox.
 * @att layout [altitem] The layout of the altitem. Can be one of 'hbox' or
 * 'vbox'.
 * @att innerwidget [altitem] The name of the widget that will produce the
 * AltComboBox value if the altitem is selected.
 */

class QFrame;
class QComboBox;
class AltComboBox : public ComboBox
{
Q_OBJECT
public:
  AltComboBox(QDomNode &node, MacroWindow *macrowindow);
  ~AltComboBox();

  virtual QString luaclass() { return LUA_CLASS; }

  QString value();
  void setValue(QString value, QString source = "");

  bool preValid();

  QComboBox *qcombobox();

  bool setKeyboardFocus();

public slots:
  void comboChanged();
  void onChildChange();

private:
  QFrame *frame;
  QComboBox *combobox;
  Widget *innerwidget;
  QString altvalue;
  Frame *altframe;
};

const struct luaL_Reg acmbbox_meths[] =
  { WDG_METHS, CMBBOX_METHS, {nullptr, nullptr} };

inline void register_altcombobox(lua_State *L)
{
  luaL_newmetatable(L, LUA_CLASS);
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);
  lua_rawset(L, -3);
  luaL_register(L, nullptr, acmbbox_meths);
}

#endif/*__PRACRO_ALTCOMBOBOX_H__*/
