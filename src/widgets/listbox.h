/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            listbox.h
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
#ifndef __PRACRO_LISTBOX_H__
#define __PRACRO_LISTBOX_H__

#include "widget.h"

#include <QDomNode>

#ifdef LUA_CLASS
#undef LUA_CLASS
#endif
#define LUA_CLASS "ListBox"

/***
 * ListBox Widget
 * @clientside
 * @tag listbox
 * @screenshot Example
 * <listbox name="x" value="item1">
 *   <item type="header" value="" caption="Numbers"/>
 *   <item value="item1" caption="Item 1"/>
 *   <item value="item2" caption="Item 2"/>
 *   <item value="item3" caption="Item 3"/>
 *   <item value="item4" caption="Item 4"/>
 *   <item value="" caption="" type="separator"/>
 *   <item type="header" value="" caption="Letters"/>
 *   <item value="itemA" caption="Item A"/>
 *   <item value="itemB" caption="Item B"/>
 *   <item value="itemC" caption="Item C"/>
 *   <item value="itemD" caption="Item D"/>
 * </listbox>
 * @extends widget
 * This widget is a list of selectable items.
 * @att value [item] The value of the item and the value of the widget if this
 * item is selected.
 * @att caption [item] The caption of the item. This is the text presented to
 * the user.
 * @att type [item] The item type. Can be either 'separator' which will show
 * the item as a line or 'header' which will highlight its caption. Ommit this
 * attribute if the item is selectable.
 */

class QListWidget;
class ListBox : public Widget
{
Q_OBJECT
public:
  ListBox(QDomNode &node, MacroWindow *macrowindow);
  ~ListBox();

  virtual QString luaclass() { return LUA_CLASS; }

  QString value();
  void setValue(QString value, QString source);

  bool preValid();
  void setWdgValid(bool valid);

  void clear();
  void addItem(QString text, QString type);

public slots:
  void changed();

private:
  bool valueIsChangingByComputer;
  QListWidget *listwidget;
};

/***
 * @method nil clear()
 * This method removes all items from the listbox.
 */
int lst_clear(lua_State *L);

/***
 * @method nil addItem(string text)
 * This method adds an item to the list.
 * @param text The item text to be added. It will be added both as the value
 * and caption of the item.
 */
int lst_add_item(lua_State *L);

/***
 * @method nil addHeader(string text)
 * This method adds a header item to the list.
 * @param text The caption of the header.
 */
int lst_add_header(lua_State *L);

/***
 * @method nil addSeparator()
 * This method adds a separator item to the list.
 */
int lst_add_separator(lua_State *L);

#define LSTBOX_METHS \
  {"clear", lst_clear},\
  {"addItem", lst_add_item}, \
  {"addHeader", lst_add_header}, \
  {"addSeparator", lst_add_separator}

const struct luaL_Reg lstbox_meths[] =
  { WDG_METHS, LSTBOX_METHS, {nullptr, nullptr} };

inline void register_listbox(lua_State *L)
{
  luaL_newmetatable(L, LUA_CLASS);
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);
  lua_rawset(L, -3);
  luaL_register(L, nullptr, lstbox_meths);
}

#endif/*__PRACRO_LISTBOX_H__*/
