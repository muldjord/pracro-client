/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lua.cc
 *
 *  Thu May 29 16:30:50 CEST 2008
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
#include "lua.h"

#include "macrowindow.h"

#include "widgets/widget.h"

#include "luawidget.h"
//#include "luadb.h"

#include "debug.h"

#define GLOBAL_POINTER "_pracroGlobalLUAObjectPointerThisShouldBeANameThatIsNotAccidentallyOverwritten"

static int get_widget(lua_State *L)
{
  int n = lua_gettop(L); // number of arguments
  if(n != 1) {
    char errstr[512];
    sprintf(errstr, "Number of args expected 1, got %d", n);
    lua_pushstring(L, errstr);
    lua_error(L);
    return 1;
  }

  QString name = lua_tostring(L, lua_gettop(L));

  lua_getglobal(L, GLOBAL_POINTER);
  LUA *lua = (LUA*)lua_touserdata(L, lua_gettop(L));

  if(!lua) {
    lua_pushstring(L, "No LUA pointer!");
    lua_error(L);
    return 1;
  }

  Widget *widget = lua->getWidget(name);

  if(widget) {
    wdg_make_widget(L, widget);
    DEBUG(lua, "get_widget found widget %s\n",
          name.toStdString().c_str());
  } else {
    lua_pushnil(L);
    WARN(lua, "get_widget could not find widget %s\n",
         name.toStdString().c_str());
  }

  return 1;
}

static int debug(lua_State *L)
{
  int n = lua_gettop(L); // number of arguments
  if(n != 1) {
    char errstr[512];
    sprintf(errstr, "Number of args expected 1, got %d", n);
    lua_pushstring(L, errstr);
    lua_error(L);
    return 1;
  }

  DEBUG(lua, "%s\n", lua_tostring(L, lua_gettop(L)));

  return 0;
}

static int get_template(lua_State *L)
{
  int n = lua_gettop(L); // number of arguments
  if(n != 0) {
    char errstr[512];
    sprintf(errstr, "Number of args expected 0, got %d", n);
    lua_pushstring(L, errstr);
    lua_error(L);
    return 1;
  }

  lua_getglobal(L, GLOBAL_POINTER);
  LUA *lua = (LUA*)lua_touserdata(L, lua_gettop(L));

  if(!lua) {
    lua_pushstring(L, "No LUA pointer!");
    lua_error(L);
    return 1;
  }

  QString templ = lua->getTemplate();
  lua_pushstring(L, templ.toStdString().c_str());

  return 1;
}

static int get_macro(lua_State *L)
{
  int n = lua_gettop(L); // number of arguments
  if(n != 0) {
    char errstr[512];
    sprintf(errstr, "Number of args expected 0, got %d", n);
    lua_pushstring(L, errstr);
    lua_error(L);
    return 1;
  }

  lua_getglobal(L, GLOBAL_POINTER);
  LUA *lua = (LUA*)lua_touserdata(L, lua_gettop(L));

  if(!lua) {
    lua_pushstring(L, "No LUA pointer!");
    lua_error(L);
    return 1;
  }

  QString macro = lua->getMacro();
  lua_pushstring(L, macro.toStdString().c_str());

  return 1;
}

static int get_user(lua_State *L)
{
  int n = lua_gettop(L); // number of arguments
  if(n != 0) {
    char errstr[512];
    sprintf(errstr, "Number of args expected 0, got %d", n);
    lua_pushstring(L, errstr);
    lua_error(L);
    return 1;
  }

  lua_getglobal(L, GLOBAL_POINTER);
  LUA *lua = (LUA*)lua_touserdata(L, lua_gettop(L));

  if(!lua) {
    lua_pushstring(L, "No LUA pointer!");
    lua_error(L);
    return 1;
  }

  QString user = lua->getUser();
  lua_pushstring(L, user.toStdString().c_str());

  return 1;
}

static int get_patientid(lua_State *L)
{
  int n = lua_gettop(L); // number of arguments
  if(n != 0) {
    char errstr[512];
    sprintf(errstr, "Number of args expected 0, got %d", n);
    lua_pushstring(L, errstr);
    lua_error(L);
    return 1;
  }

  lua_getglobal(L, GLOBAL_POINTER);
  LUA *lua = (LUA*)lua_touserdata(L, lua_gettop(L));

  if(!lua) {
    lua_pushstring(L, "No LUA pointer!");
    lua_error(L);
    return 1;
  }

  QString patientid = lua->getPatientID();
  lua_pushstring(L, patientid.toStdString().c_str());

  return 1;
}

LUA::LUA(Widget **rootwidget, QString templ, QString macro, QString user,
         QString patientid)
{
  this->rootwidget = rootwidget;
  this->templ = templ;
  this->macro = macro;
  this->user = user;
  this->patientid = patientid;
  L = nullptr;
  clear();
}

LUA::~LUA()
{
  lua_close(L);
}

void LUA::clear()
{
  if(L) lua_close(L);

  L = luaL_newstate();
  if(L == nullptr) {
    ERR(lua, "Could not create LUA state.\n");
    return;
  }

  luaL_openlibs(L);               

  lua_pushlightuserdata(L, this); // Push the pointer to 'this' instance
  lua_setglobal(L, GLOBAL_POINTER); // Assign it to a global lua var.

  lua_register(L, "widget", get_widget);
  lua_register(L, "debug", debug);
  lua_register(L, "template", get_template);
  lua_register(L, "macro", get_macro);
  lua_register(L, "user", get_user);
  lua_register(L, "patientid", get_patientid);

  register_widgets(L);
  //register_db(L);
}

QString LUA::runScriptS(QString script, Widget *widget, QString name)
{
  if(L == nullptr) {
    ERR(lua, "LUA state not initialized!\n");
    return "";
  }

  int top = lua_gettop(L);

  DEBUG(lua, "Running %s script %s on %s widget.\n",
        name.toStdString().c_str(),
        script.toStdString().c_str(),
        widget?widget->name().toStdString().c_str():"nullptr");

  if(widget) {
    wdg_make_widget(L, widget);
    lua_setglobal(L, "this");
  }

  if(luaL_loadbuffer(L,
                     script.toStdString().c_str(),
                     script.toStdString().size(),
                     name.toStdString().c_str())) {
    ERR(lua, "%s\n", lua_tostring(L, lua_gettop(L)));
    return "";
  }

  // Run the loaded code
  if(lua_pcall(L, 0, LUA_MULTRET, 0)) {
    ERR(lua, "%s\n", lua_tostring(L, lua_gettop(L)));
    return "";
  }

  if(top != lua_gettop(L) - 1) {
    ERR(lua, "Program did not return a single value.\n");
    return "";
  }

  if(lua_isstring(L, lua_gettop(L)) == false) {
    ERR(lua, "Program did not return a boolean value.\n");
    return "";
  }

  QString res = lua_tostring(L, lua_gettop(L));
  lua_pop(L, 1);

  return res;
}

bool LUA::runScript(QString script, Widget *widget, QString name)
{
  if(L == nullptr) {
    ERR(lua, "LUA state not initialized!\n");
    return false;
  }

  DEBUG(lua, "Running %s script %s on %s widget.\n",
        name.toStdString().c_str(),
        script.toStdString().c_str(),
        widget?widget->name().toStdString().c_str():"nullptr");

  if(widget) {
    wdg_make_widget(L, widget);
    lua_setglobal(L, "this");
  }

  if(luaL_loadbuffer(L,
                     script.toStdString().c_str(),
                     script.toStdString().size(),
                     name.toStdString().c_str())) {
    ERR(lua, "%s\n", lua_tostring(L, lua_gettop(L)));
    return false;
  }

  // Run the loaded code
  if(lua_pcall(L, 0, LUA_MULTRET, 0)) {
    ERR(lua, "%s\n", lua_tostring(L, lua_gettop(L)));
    return false;
  }

  return true;
}

Widget *LUA::getWidget(QString name)
{
  if(*rootwidget) return (*rootwidget)->findWidget(name, true);
  else {
    WARN(lua, "Could not find widget '%s'\n", name.toStdString().c_str());
    return nullptr;
  }
}

QString LUA::getTemplate()
{
  return templ;
}

QString LUA::getMacro()
{
  return macro;
}

QString LUA::getUser()
{
  return user;
}

QString LUA::getPatientID()
{
  return patientid;
}
