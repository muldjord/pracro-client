/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            luadb.h
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
#ifndef __PRACRO_LUADB_H__
#define __PRACRO_LUADB_H__

#include <lua.hpp>

/***
 * Database Class
 * @class DB
 * @clientside
 * The DB class connects and handles communication with the an SQL server using
 * the QDatabase API.
 */

/***
 * @method object DB.new(string driver, string host, string database, string user, string password, int port)
 * Create a new DB connection object.
 * @param driver A string containing the QT SQL driver name. QPSQL: postgresql
 * driver.
 * @param host The hostname to connect to.
 * @param database The database to open.
 * @param user The username to use for authentication.
 * @param password The password to use for authentication.
 * @param port The port to connect to.
 * @return The newly created communication object.
 * @example Create a new database object:
 * px = DB.new('QPSQL', 'localhost', 'mydb', 'user01', 'verysecret', '5432')
 */
int db_new(lua_State *L);

/***
 * @method nil exec(string query)
 * Execute an SQL query.
 * @param query A string containing the SQL query to execute.
 */ 
int db_exec(lua_State *L);

/***
 * @method string value(int index)
 * Get current value from the result of the last query.
 * @param index The column index number to use for the value retrieval.
 * @return A string containing the value in the result cell.
 */
int db_value(lua_State *L);

/***
 * @method boolean next()
 * Iterate to next row in result set.
 * @return true if another row is availble, false if the end of the list has
 * been reached.
 * @example Iterate through a result list and print the value in column 0 in each row:
 * while db:next()
 * do
 *   local item = db:value(0)
 *   print(item)
 * end
 */
int db_next(lua_State *L);

void register_db(lua_State *L);

#endif/*__PRACRO_LUADB_H__*/
