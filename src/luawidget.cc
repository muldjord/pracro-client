/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            luawidget.cc
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
#include "luawidget.h"

#include "widgets.h"

#include "debug.h"

int wdg_make_widget(lua_State *L, Widget *widget)
{
  wdg_userdata *wdgu;
  wdgu = (wdg_userdata *)lua_newuserdata(L, sizeof(wdg_userdata));
  luaL_getmetatable(L, widget->luaclass().toStdString().c_str());

  lua_setmetatable(L, -2);

  wdgu->widget = widget;

  return 1;
}

void register_widgets(lua_State *L)
{
  register_widget(L);

  register_checkbox(L);
  register_checkgroupbox(L);

  register_combobox(L);
  register_altcombobox(L);

  register_listbox(L);
  register_lineedit(L);
}

