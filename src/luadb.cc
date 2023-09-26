/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            luadb.cc
 *
 *  Sat Aug 28 13:40:58 CEST 2010
 *  Copyright 2010 Bent Bisballe Nyeng
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
#include "luadb.h"

#include <QSqlDatabase>
#include <QSqlQuery>

#include <lauxlib.h>

#include "debug.h"

#define luaL_checkbool(L, i) \
  (lua_isboolean(L,i) ? lua_toboolean(L,i) : luaL_checkint(L,i))

class DB {
public:
  DB(QString driver, QString server, QString database,
     QString user, QString password = "", int port = -1) {
    db = QSqlDatabase::addDatabase(driver);
    db.setHostName(server);
    if(port != -1) db.setPort(port);
    db.setDatabaseName(database);
    db.setUserName(user);
    if(password != "") db.setPassword(password);
    db.setConnectOptions("connect_timeout=2000");
    db.open();
  }

  ~DB() {
    db.close();
  }
  
  void exec(const QString &querystr) {
    query = db.exec(querystr);
  }

  bool next()
  {
    return query.next();
  }

  QString value(int idx) {
    return query.value(idx).toString();
  }

private:
  QSqlDatabase db;
  QSqlQuery query;
};

typedef struct db_userdata {
  DB *db;
} db_userdata;

int db_value(lua_State *L)
{
  db_userdata *dbu;

  dbu = (db_userdata *)luaL_checkudata(L, 1, "DB");
  luaL_argcheck(L, dbu, 1, "DB expected");

  int idx = luaL_checkinteger(L, 2);

  lua_pushstring(L, dbu->db->value(idx).toStdString().c_str());

  return 1;
}

int db_exec(lua_State *L)
{
  db_userdata *dbu;

  dbu = (db_userdata *)luaL_checkudata(L, 1, "DB");
  luaL_argcheck(L, dbu, 1, "DB expected");

  const char *query = luaL_checkstring(L, 2);

  dbu->db->exec(query);

  return 0;
}

int db_next(lua_State *L)
{
  db_userdata *dbu;

  dbu = (db_userdata *)luaL_checkudata(L, 1, "DB");
  luaL_argcheck(L, dbu, 1, "DB expected");

  bool more = dbu->db->next();

  lua_pushboolean(L, more);

  return 1;
}

int db_new(lua_State *L)
{
  const char *driver = luaL_checkstring(L, 1);
  const char *host = luaL_checkstring(L, 2);
  const char *database = luaL_checkstring(L, 3);
  const char *user = luaL_checkstring(L, 4);
  const char *password = luaL_checkstring(L, 5);
  int port = (int)luaL_checknumber(L, 6);

  db_userdata *dbu;
  dbu = (db_userdata *)lua_newuserdata(L, sizeof(db_userdata));

  luaL_getmetatable(L, "DB");
  lua_setmetatable(L, -2);

  dbu->db = new DB(driver, host, database, user, password, port);

  return 1;
}

static int db_gc(lua_State *L)
{
  db_userdata *dbu;

  dbu = (db_userdata *)luaL_checkudata(L, 1, "DB");
  luaL_argcheck(L, dbu, 1, "DB expected");

  delete dbu->db;

  return 0;
}

static const struct luaL_Reg db_meths[] = {
  //  {"__new", db_new},
  {"__gc" ,db_gc},
  {"value", db_value},
  {"next", db_next},
  {"exec", db_exec},
  {nullptr, nullptr}
};

static const struct luaL_reg db_funcs[] = {
  {"new", db_new},
  {nullptr, nullptr}
};

void register_db(lua_State *L)
{
  luaL_newmetatable(L, "DB");
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);
  lua_rawset(L, -3);
  luaL_register(L, nullptr, db_meths);
  luaL_openlib (L, "DB", db_funcs, 0);
}

/*

Example:
--------
db = DB.new(...)
db:exec('...')

while db:next()
do
  val0 = db:value(0)
  val1 = db:value(1)
  ...
end

 */
