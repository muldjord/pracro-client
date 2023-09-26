/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            combobox.h
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
#ifndef __PRACRO_COMBOBOX_H__
#define __PRACRO_COMBOBOX_H__

#include <QDomNode>
#include <QRegExp>

#include "widget.h"

#ifdef LUA_CLASS
#undef LUA_CLASS
#endif
#define LUA_CLASS "ComboBox"

/***
 * ComboBox Widget
 * @tag combobox
 * @clientside
 * @screenshot Example
 * <combobox name="ex" layout="vbox" value="item" type="select">
 *   <item caption="Item" value="item"/>
 * </combobox>
 * @extends widget
 * The ComboBox is used to make a selection from a list of items.
 * The ComboBox contains <code>&lt;item&gt;</code> tags each describing 
 * an entry in the selection list.
 * @att type The selection method of the ComboBox. Can be one 'select', where
 * the user can select items using either the mouse or the arrow keys, 'edit',
 * where the user can write freely in the ComboBox in the same way as in a
 * LineEdit during which it will present but not limit to the matching values
 * of its list, and finally 'search', where the user can write in the ComboBox
 * but is restricted to writing strings matching the items in the list.
 * @att value [item] The value of the item. This will be the value of the
 * ComboBox if this item is selected.
 * @att caption [item] The caption of this item. This is the text presented
 * to the user in the ComboBox.
 */

typedef enum {
  SELECT,
  EDIT,
  SEARCH
} types_t;

class QComboBox;
class QWheelEvent;
class ComboBox : public Widget
{
Q_OBJECT
public:
  ComboBox(QDomNode &node, MacroWindow *macrowindow);
  ~ComboBox();

  virtual QString luaclass() { return LUA_CLASS; }

  QString value();
  void setValue(QString value, QString source = "");

  QString lineEditValue();
  void setLineEditValue(QString value);

  bool preValid();

  void setWdgValid(bool valid);

  void clear();
  void addItem(QString item, QString value);

public slots:
  void changed();

protected:
  bool eventFilter(QObject *obj, QEvent *event);
  //  void wheelEvent(QWheelEvent *);
  void changeEvent(QEvent *event);

private:
  void updateSearchList();

  QRegExp rx;
  types_t combotype;
  bool ischangingbyuser;

  QComboBox *combobox;
  bool ignoreChangeEvents;

  QStringList itemlist;
};

/***
 * @method nil clear()
 * This method removes all items from the combobox.
 */
int cmb_clear(lua_State *L);

/***
 * @method nil addItem(string caption, string value)
 * This method adds an item to the selectionlist of the combobox.
 * @param caption The item text to be added. It will be added both as the value
 * and caption of the item.
 * @param value The item value. This value is optional. If left out, the
 * caption will be used as the item value.
 */
int cmb_add_item(lua_State *L);

/***
 * @method string lineEditValue()
 * This method is used to get the value from the combobox lineedit (in case it
 * is in edit mode).
 * @return a string containing the current value of the combobox lineedit.
 */
int cmb_le_value(lua_State *L);

/***
 * @method nil setLineEditValue(string text)
 * This method is used to set the value of the combobox lineedit (in case it
 * is in edit mode).
 * @param text The text string to be set.
 */
int cmb_le_set_value(lua_State *L);

#define CMBBOX_METHS \
  {"clear", cmb_clear},\
  {"addItem", cmb_add_item},\
  {"lineEditValue", cmb_le_value},\
  {"setLineEditValue", cmb_le_set_value}

const struct luaL_Reg cmbbox_meths[] =
  { WDG_METHS, CMBBOX_METHS, {nullptr, nullptr} };

inline void register_combobox(lua_State *L)
{
  luaL_newmetatable(L, LUA_CLASS);
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);
  lua_rawset(L, -3);
  luaL_register(L, nullptr, cmbbox_meths);
}

#endif/*__PRACRO_COMBOBOX_H__*/
