/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            window.h
 *
 *  Wed Jul 18 11:44:51 CEST 2007
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
#ifndef __PRACRO_WINDOW_H__
#define __PRACRO_WINDOW_H__

#include "widget.h"
#include <QDomNode>

/***
 * Widgets Outer Container
 * @tag widgets
 * @container
 * @clientside
 * This is the outer tag of the macro widgets.
 * @att caption This is the name that will be shown in the macro.
 * @att layout this is the layout that is used by the macro. It can be one of
 * 'hbox' or 'vbox'.
 */
class Window : public Widget
{
public:
  Window(QDomNode &node, MacroWindow *macrowindow);
  ~Window();

  QString value() { return ""; }
  void setValue(QString, QString) {}

  void setWdgValid(bool) {}

  bool setKeyboardFocus();
};

#endif/*__PRACRO_WINDOW_H__*/
