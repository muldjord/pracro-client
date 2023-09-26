/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            luawidget.h
 *
 *  Tue Aug  3 10:17:02 CEST 2010
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
#ifndef __PRACRO_LUAWIDGET_H__
#define __PRACRO_LUAWIDGET_H__

#include <lua.hpp>
//#include <lauxlib.h>

#include "widgets/widget.h"

#define LUA_SRC "lua"

//void *luaL_isudata (lua_State *L, int ud, const char *tname);

#define luaL_checkbool(L, i) \
  (lua_isboolean(L,i) ? lua_toboolean(L,i) : luaL_checkint(L,i))

typedef struct wdg_userdata {
  Widget *widget;
} wdg_userdata;

void register_widgets(lua_State *L);
int wdg_make_widget(lua_State *L, Widget *widget);

#endif/*__PRACRO_LUAWIDGET_H__*/
