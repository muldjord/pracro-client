/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            macro.h
 *
 *  Tue Jun 30 11:49:46 CEST 2009
 *  Copyright 2009 Bent Bisballe Nyeng
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
#ifndef __PRACRO_MACRO_H__
#define __PRACRO_MACRO_H__

#include <QString>
#include <QStringList>
#include <QMap>
#include <QDomNode>
#include <QBoxLayout>

#include <QScrollArea>

#include "netcom.h"
#include "entity.h"

class MacroWindow;
class MacroDrawer;

class Macro : public Entity {
public:
  Macro(Entities &entities, NetCom &netcom,
        QString course, QString templ,
        QScrollArea *scrollarea);
  virtual ~Macro() {}

  const char *type();

  void update(QDomNode &node);

  QWidget *widget();

  bool isOpen();

  QString name;
  QString caption;

  // Dependency system
  QStringList requiresmacro;
  //  bool isrequired;
  bool iscompleted;

  bool isstatic;
  bool iscompact;
  bool isimportant;

  MacroWindow *window;
  MacroDrawer *drawer;

private:
  QScrollArea *scrollarea;
  Entities &entities;
  NetCom &netcom;

  QString course;
  QString templ;
};

#endif/*__PRACRO_MACRO_H__*/
