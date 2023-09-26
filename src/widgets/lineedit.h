/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lineedit.h
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
#ifndef __PRACRO_LINEEDIT_H__
#define __PRACRO_LINEEDIT_H__

#include <QStringList>

#include "widget.h"

#ifdef LUA_CLASS
#undef LUA_CLASS
#endif
#define LUA_CLASS "LineEdit"

/***
 * Line Edit Widget
 * @tag lineedit
 * @clientside
 * @screenshot Example
 * <lineedit name="myname" value="some text"/>
 * @extends widget
 * The lineedit is a single line textual input field.
 * @example Simple example of usage:
 * <lineedit name="myname" value="some text"/>
 * @example A more advanced example:
 * <lineedit name="myname" value="some text" readonly="true"
 *           onChange="this:setValue('['..this:value()..']')"/>
 * @att readonly Make the lineedit readonly (not changeable by the user), but
 * still able to select and copy text.
 */

class QLineEdit;
class QEvent;
class LineEdit : public Widget
{
Q_OBJECT
public:
  LineEdit(QDomNode &node, MacroWindow *macrowindow);
  ~LineEdit();

  virtual QString luaclass() { return LUA_CLASS; }

  QString value();
  void setValue(QString value, QString source);

  bool readOnly() override;
  void setReadOnly(bool readOnly) override;

  void setWdgValid(bool valid);

  void clearSuggestions();
  void addSuggestion(QString suggestion);
  bool isSuggested(QString value);
  void showSuggestions();

public slots:
  void changed();
  void user_changed();

protected:
  void changeEvent(QEvent *event);

private:
  bool eventFilter(QObject *, QEvent *event);

  QLineEdit *lineedit;
  QStringList suggestions;
};

/***
 * @method nil clearSuggestions()
 * This method clears the suggestion list.
 * For an example see @see showSuggestions().
 */
int lin_clear_suggestions(lua_State *L);

/***
 * @method nil showSuggestions()
 * This method forces showing of the suggestion list.
 * @example Example of suggestion usage:
 * function myOnChange()
 *   if(isSuggested(this:value()) == false)
 *   then
 *     this:clearSuggestions()
 *     this:addSuggestion(this:value() .. 'abc')
 *     this:addSuggestion(this:value() .. 'abd')
 *     this:addSuggestion(this:value() .. 'acd')
 *     this:showSuggestions()
 *   end
 * end
 */
int lin_show_suggestions(lua_State *L);

/***
 * @method boolean isSuggested(string value)
 * This method makes a lookup in the suggestion list, searching for value.
 * For an example see @see showSuggestions().
 * @param value A string containing the text to look for in the suggestion list.
 * @return a boolean with the value true if the string was found, false
 * if not.
 */
int lin_is_suggested(lua_State *L);

/***
 * @method nil addSuggestion(string suggestion)
 * Adds a suggestion to the suggestion list. The list is popped up each time 
 * a character is entered into the lineedit by the user, or it is explicitly
 * opened by calling @see showSuggestions().
 * For an example see @see showSuggestions().
 * @param suggestion A string containing the value to be added to the
 * suggestion list.
 */
int lin_add_suggestion(lua_State *L);

#define LINEDT_METHS \
  {"clearSuggestions", lin_clear_suggestions},  \
  {"showSuggestions", lin_show_suggestions},\
  {"isSuggested", lin_is_suggested},\
  {"addSuggestion", lin_add_suggestion}

const struct luaL_Reg linedt_meths[] =
  { WDG_METHS, LINEDT_METHS, {nullptr, nullptr} };

inline void register_lineedit(lua_State *L)
{
  luaL_newmetatable(L, LUA_CLASS);
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);
  lua_rawset(L, -3);
  luaL_register(L, nullptr, linedt_meths);
}

#endif/*__PRACRO_LINEEDIT_H__*/
