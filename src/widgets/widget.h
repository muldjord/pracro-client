/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            widget.h
 *
 *  Tue Jul 17 12:15:59 CEST 2007
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
#ifndef __PRACRO_WIDGET_H__
#define __PRACRO_WIDGET_H__

#include <QString>
#include <QDomNode>
#include <QRegExp>
#include <QObject>
#include <QVector>
#include <QFrame>
#include <QAction>
#include "shortcuttooltip.h"

#include "lua.h"

#ifdef LUA_CLASS
#undef LUA_CLASS
#endif
#define LUA_CLASS "Widget"

/***
 * Virtual Base Widget
 * This tag is purely virtual. It is inherited by all other widgets.
 * @clientside
 * @att name The name of the widget. This is also the name used by the scripts.
 * @att value The initial value of the widget. It is overwritten if there is a
 * map with a recent value or if the database contains a recent value. 
 * @att onChange Change event script callback. This attribute contains script
 *  code that are executed each time the widget is changed.
 * @att onInit Init event script callback. This attribute contains script code
 *  that are executed when the widget has just been created.
 * @att map This attribute binds the value of the widget to a map.
 * @att width Use this attribute to set the width of the widget to a fixed
 *  value.
 * @att height Use this attribute to set the height of the widget to a fixed
 *  value.
 * @att help This attribute contains a help text for this widget. It will be
 *  shown to the user when the mouse is hovering over the widget.
 * @att local This attribute is a boolean telling wether the field can be stored
 *  in the database on commits. It can be either 'true' or 'false'. The default
 *  is 'false'.
 * @att colour The foreground (text) colour of the widget. The value must be a
 * 6 byte hexadecimal string with each 2 byte pair describing the colour
 * channels in order: Red, green blue. Example: '0f1a3b'.
 * @att bgcolour The background colour of the widget. The value must be a
 * 6 byte hexadecimal string with each 2 byte pair describing the colour
 * channels in order: Red, green blue. Example: '0f1a3b'.
 */

class QLayout;
class MacroWindow;
class LUA;
class Widget;
class Widget : public QObject {
Q_OBJECT
public:
  Widget(QDomNode &node, MacroWindow *macrowindow);
  virtual ~Widget();

  QString name();
  QString type();
  bool local();

  virtual QString luaclass() { return LUA_CLASS; }

  virtual QString value() = 0;
  virtual void setValue(QString value, QString source) = 0;

  virtual bool readOnly() { return false; };
  virtual void setReadOnly(bool) {};

  // Set deep to true to validate inner widgets.
  bool valid(bool deep = false);
  void setValid(bool valid, bool deep = false);

  // Make sanity check before commit.
  bool committable(bool deep = false);

  // Implement in subclasses to contribute to the validation.
  virtual bool preValid() { return true; }

  // Implement in subclasses to change widget according to validation status.
  virtual void setWdgValid(bool valid) = 0;

  virtual void setEnabled(bool enabled);
  virtual bool enabled();

  virtual void setVisible(bool visible);
  virtual bool visible();

  virtual void setHidden(bool hidden);
  virtual bool hidden();

  virtual bool setKeyboardFocus();
  bool setKeyboardFocusRecursive();

  virtual void setForegroundColour(unsigned char red,
                                   unsigned char green,
                                   unsigned char blue);
  virtual void setBackgroundColour(unsigned char red,
                                   unsigned char green,
                                   unsigned char blue);

  virtual QWidget *qwidget() { return widget; }
  
  // Set deep to true to search through inner widgets.
  Widget *findWidget(QString name, bool deep = false);
  QVector< Widget* > widgetList(bool deep = false);
  void addChild(Widget *widget);

  virtual void setValues();

  bool eventFilter(QObject *obj, QEvent *event);

signals:
  void wasChanged();

  /*
   * LUA scripting events:
   */
  void eventOnChange(bool deep = false);
  void eventOnInit(bool deep = false);

public slots:
  void childWasChanged();
  void runEventOnChange(bool deep = false);
  void runEventOnInit(bool deep = false);
  virtual void shortcutActivated();

protected:
  void setWidget(QWidget *widget);
  QWidget *getWidget();

  bool hideChildren;

  // Implement in subclasses to be able to block commits.
  virtual bool preCommit() { return true; }

  // Overload this to avoid children 'wasChanged' connection.
  virtual void connectChild(Widget *child);
  void addChildren(QDomNode &xml_node, QLayout *layout);

  LUA *lua;

  void createWidget(QDomNode &xml_node, QLayout *layout);
  QVector< Widget* > children;

  void setWdgValidRecursive(bool valid);

  // Store value in constructor to be set later.
  bool has_lazy;
  QString lazy_value;
  QString lazy_source;

  bool is_valid;
  QString widget_name;
  QString widget_type;
  bool widget_local;

  MacroWindow *macrowindow;

  bool hasOnChangeEvent;
  QString onChangeEventScript;

  bool hasOnInitEvent;
  QString onInitEventScript;

  QAction *shortcut_action;

  QFrame *replwidget;

private:
  QWidget *widget;
  bool important;
  ShortcutTooltip *shortcut_label;
};

/***
 * @method string name()
 * This method is used to get the name of the widget. This name is the same as
 * the one specified in the <code>name</code> attribute.
 * @return a string containing the name of the widget.
 */
int wdg_name(lua_State *L);

/***
 * @method string type()
 * This method is used to get the type of the widget. This type is the same as
 * the widget tagname and can be used by script to verify the type before
 * calling any type specific methods.
 * @return a string containing the type of the widget.
 */
int wdg_type(lua_State *L);

/***
 * @method string value()
 * This method is used to get the current value of the widget.
 * @return a string containing the value of the widget.
 */
int wdg_value(lua_State *L);

/***
 * @method nil setValue(string value)
 * This method is used to set the value of the widget.
 * @param value A string containing the value to set.
 */
int wdg_set_value(lua_State *L);

/***
 * @method boolean readOnly()
 * This method is used to get the current read only state of the widget.
 * Currently only implemented for lineedit and textedit widget types.
 * @return a boolean value. If the widget is read only the method returns true
 * otherwise it returns false.
 */
int wdg_readonly(lua_State *L);

/***
 * @method nil setReadOnly(boolean readOnly)
 * This method is used to set a widget read only.
 * Currently only implemented for lineedit and textedit widget types.
 * @param readOnly A boolean value. If true the widget is set read only. If
 * false the widget value is editable by the user.
 */
int wdg_set_readonly(lua_State *L);

/***
 * @method boolean enabled()
 * This method is used to get the current enable state of the widget.
 * @return a boolean value. If the widget is enabled the method returns true
 * otherwise it returns false.
 */
int wdg_enabled(lua_State *L);

/***
 * @method nil setEnabled(boolean enabled)
 * This method is used to either enable (make editable) or disable (grey out)
 * the widget. <em>NOTE</em>: A disabled widget will not supply its value to
 * the server during a commit.
 * @param enabled A boolean value. If true the widget is enabled. If false the
 * widget is disabled.
 */
int wdg_set_enabled(lua_State *L);

/***
 * @method boolean visible()
 * This method is used to get the current visibility state of the widget.
 * @return a boolean value. If the widget is visible the method returns true
 * otherwise it returns false.
 */
int wdg_visible(lua_State *L);

/***
 * @method nil setVisible(boolean visible)
 * This method is used to either show or hide the widget. 
 * The widget will take up an equal amount of layout space regardless of its
 * visibility state meaning that the layout will remain static after a
 * hiding/showing of the widget.
 * <em>NOTE</em>: An invisble widget will not supply its value to the server
 * during a commit.
 * @param visible A boolean value. If true the widget is shown. If false the
 * widget is hidden.
 */
int wdg_set_visible(lua_State *L);

/***
 * @method boolean hidden()
 * This method is used to get the current hide state of the widget.
 * @return a boolean value. If the widget is hidden the method returns true
 * otherwise it returns false.
 */
int wdg_hidden(lua_State *L);

/***
 * @method nil setHidden(boolean hidden)
 * This method is used to either show or hide the widget, collapsing the occupied
 * space.
 * <em>NOTE</em>: A hidden widget will not supply its value to the server
 * during a commit.
 * @param hidden A boolean value. If true the widget is hidden. If false the
 * widget is shown.
 */
int wdg_set_hidden(lua_State *L);

/***
 * @method boolean valid()
 * This method is used to get the current validity state of the widget. See
 * also @see setValid().
 * @return a boolean value. If the widget is valid the method returns true
 * otherwise it returns false.
 */
int wdg_valid(lua_State *L);

/***
 * @method nil setValid(boolean valid)
 * This method is used to set the widgets validity state. Most widgets have a
 * visual indication of their validity state (a red background colour for
 * example) and this will also be set using this method. See also @see valid().
 * <em>NOTE</em>: An invalid widget that are not an inner widget will block a
 * server commit.
 * @param valid A boolean value. If true the widgets validity state is set to
 * 'valid'. If false the widgets validity state is set to 'invalid'.
 */
int wdg_set_valid(lua_State *L);

/***
 * @method nil setBackgroundColour(integer red, integer green, integer blue)
 * This method is used to change to background colour of the widget.
 * @param red An integer value in the range 0-255. This is the red component of
 * the RGB colour value.
 * @param green An integer value in the range 0-255. This is the green component
 * of the RGB colour value.
 * @param blue An integer value in the range 0-255. This is the blue component
 * of the RGB colour value.
 */
int wdg_set_bgcol(lua_State *L);

/***
 * @method nil setForegroundColour(integer red, integer green, integer blue)
 * This method is used to change to foreground (text) colour of the widget.
 * @param red An integer value in the range 0-255. This is the red component of
 * the RGB colour value.
 * @param green An integer value in the range 0-255. This is the green component
 * of the RGB colour value.
 * @param blue An integer value in the range 0-255. This is the blue component
 * of the RGB colour value.
 */
int wdg_set_fgcol(lua_State *L);

#define WDG_METHS \
  {"name", wdg_name},\
  {"type", wdg_type},\
  {"value", wdg_value},\
  {"setValue", wdg_set_value},\
  {"readOnly", wdg_readonly},\
  {"setReadOnly", wdg_set_readonly},\
  {"enabled", wdg_enabled},\
  {"setEnabled", wdg_set_enabled},\
  {"visible", wdg_visible},\
  {"setVisible", wdg_set_visible},\
  {"hidden", wdg_hidden},\
  {"setHidden", wdg_set_hidden},\
  {"valid", wdg_valid},\
  {"setValid", wdg_set_valid},\
  {"setForegroundColour", wdg_set_fgcol},\
  {"setBackgroundColour", wdg_set_bgcol}

const struct luaL_Reg wdg_meths[] =
  { WDG_METHS, {nullptr, nullptr} };

inline void register_widget(lua_State *L)
{
  luaL_newmetatable(L, LUA_CLASS);
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);
  lua_rawset(L, -3);
  luaL_register(L, nullptr, wdg_meths);
}

#endif/*__PRACRO_WIDGET_H__*/
