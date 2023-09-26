/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            macrowindow.h
 *
 *  Fri Aug 31 12:27:45 CEST 2007
 *  Copyright 2007 Lars Bisballe Jensen and Bent Bisballe Nyeng
 *  larsbisballe@gmail.com and deva@aasimon.org
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
#ifndef __PRACRO_MACROWINDOW_H__
#define __PRACRO_MACROWINDOW_H__

#include <QDomNode>
#include <QVector>

#include "collapser.h"
#include "netcom.h"
#include "resumewidget.h"

class LUA;
class Widget;
class MacroDrawer;
class MacroWindow : public Collapser
{
Q_OBJECT
public:
  MacroWindow(NetCom &netcom, QString course, QString templ,
              bool is_static, bool compact,
              QScrollArea *scrollarea,
              MacroDrawer *drawer);
  ~MacroWindow();
  LUA *lua;
  void update(QDomNode &xml_doc);
  void setActive(bool active);
  ResumeWidget *resumewidget;
  QString macrotitle;

public slots:
  void commit();
  void cancel();

  void toggleMacro();

  void macroChanged();

  void collapseWrapper();
  void expandWrapper();

signals:
  void updateOnCommit();
  void macroHasChanged();
  void activationChanged(bool);

private slots:
  void animated(QWidget*);

private:
  void initMacro(QDomNode &node);
  void updateResume(QDomNode &node);

  bool doCommit();

  QString course;
  QString templ;
  QString macro;

  QString version;
  Widget *mainwidget;

  void clear();

  NetCom &netcom;

  bool waschanged;
  bool active;

  bool is_static;

  QWidget *scrollarea;

public:
  MacroDrawer *drawer;
};

#endif/*__PRACRO_MACROWINDOW_H__*/
