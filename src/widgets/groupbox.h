/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            groupbox.h
 *
 *  Wed Jul 18 09:56:36 CEST 2007
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
#ifndef __PRACRO_GROUPBOX_H__
#define __PRACRO_GROUPBOX_H__

#include "widget.h"

#include <QDomNode>

/*
 * NOTE: This documentation is both for the groupbox and frame classes.
 */
/***
 * Layout or Frame Widget 
 * @tag frame
 * @clientside
 * @screenshot Example with caption.
 * <frame caption="Caption" layout="vbox">
 *   <label caption="Label"/>
 *   <lineedit name="x" value="LineEdit"/>
 * </frame>
 * @screenshot Example without caption (frame is invisible).
 * <frame layout="vbox">
 *   <label caption="Label"/>
 *   <lineedit name="x" value="LineEdit"/>
 * </frame>
 * @extends widget
 * This widget is used to control the layout of the child widgets and/or produce
 * a frame and caption for them. <em>NOTE</em>: The value of this widget is
 * always the empty string. It cannot be set.
 * @att layout The layout assigned to the frame. Can be one of 'hbox' or
 * 'vbox'.
 * @att caption The optional caption of the frame. (leave this out to draw
 * without the frame and caption). En empty caption attribute will produce a
 * nameless frame.
 */
class QGroupBox;
class GroupBox : public Widget
{
public:
  GroupBox(QDomNode &node, MacroWindow *macrowindow);
  ~GroupBox();

  QString value() { return ""; }
  void setValue(QString, QString) {}

  void setWdgValid(bool) {}

  bool setKeyboardFocus();

private:
  QGroupBox *groupbox;
};

#endif/*__PRACRO_GROUPBOX_H__*/
