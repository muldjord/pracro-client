/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lua.h
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
#ifndef __PRACRO_LUA_H__
#define __PRACRO_LUA_H__

#include <lua.hpp>
#include <lauxlib.h>

#include <QString>
#include <QVector>

class Widget;
class LUA {
public:
  LUA(Widget **rootwidget, QString templ, QString macro, QString user,
      QString patientid);
  ~LUA();
  
  bool runScript(QString script, Widget *widget, QString name = "");
  QString runScriptS(QString script, Widget *widget, QString name = "");

  Widget *getWidget(QString name);

  void clear();

  QString getTemplate();
  QString getMacro();
  QString getUser();
  QString getPatientID();

  QString macro;

private:
  lua_State *L;
  Widget **rootwidget;

  QString templ;
  QString user;
  QString patientid;
};

#endif/*__PRACRO_LUA_H__*/
