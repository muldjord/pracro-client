/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            button.h
 *
 *  Fri Jul 13 12:38:45 CEST 2007
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
#ifndef __PRACRO_BUTTON_H__
#define __PRACRO_BUTTON_H__

#include "widget.h"

#include <QDomNode>

/***
 * PushButton Widget
 * @tag button
 * @clientside
 * @screenshot Example button
 * <button caption="Click me"/>
 * @screenshot Example cancel button
 * <button caption="Cancel" action="cancel"/>
 * @screenshot Example commit button (inactive)
 * <button caption="Commit" action="commit"/>
 * @extends widget
 * This is a normal pushbutton that can be clicked by the user.
 * @att caption The caption of the button.
 * @att action The action of the button. 'commit' commits tha macro when
 * clicked, 'cancel' cancels (close without saving) the macro when clicked.
 */
class QPushButton;
class Button : public Widget
{
Q_OBJECT
public:
  Button(QDomNode &node, MacroWindow *macrowindow);
  ~Button();

  QString value() { return ""; }
  void setValue(QString, QString) {}
  void setWdgValid(bool) {}

public slots:
  void commit();
  void cancel();
  void do_enable();
  void shortcutActivated();

signals:
  void act_commit();
  void act_cancel();

private:
  QPushButton *button;
};
#endif/*__PRACRO_BUTTON_H__*/
