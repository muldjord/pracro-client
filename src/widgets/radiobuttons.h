/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            radiobuttons.h
 *
 *  Wed Jul 18 10:35:52 CEST 2007
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
#ifndef __PRACRO_RADIOBUTTONS_H__
#define __PRACRO_RADIOBUTTONS_H__

#include <QDomNode>
#include <QVector>

#include "widget.h"

/***
 * Radio Button Group Widget
 * @tag radiobuttons
 * @clientside
 * @extends widget
 * @screenshot Example with four radiobuttons
 * <radiobuttons name="x" value="item2">
 *   <radiobutton value="item1" caption="Item 1"/>
 *   <radiobutton value="item2" caption="Item 2"/>
 *   <radiobutton value="item3" caption="Item 3"/>
 *   <radiobutton value="item4" caption="Item 4"/>
 * </radiobuttons>
 * @container
 * This widget is in many ways similar to the @see listbox widget. It is a
 * container for radiobuttons represented by <code>&lt;radiobutton&gt;</code>
 * tags. The value of the widget is retrieved from the selected radiobutton.
 * When the value of the radiobuttons widget is set the corresponding
 * radiobutton will be selected.
 * @att layout the layout of the widget. Can be one of 'vbox' or 'hbox'.
 * @att caption [radiobutton] This is the caption of the radio button.
 * @att value [radiobutton] This is the value of the radiobutton, which will
 * be the value of the radiobuttons widget should this radiobutton be selected.
 */

class QFrame;
class QEvent;
class RadioButton;
class RadioButtons : public Widget
{
Q_OBJECT
public:
  RadioButtons(QDomNode &node, MacroWindow *macrowindow);
  ~RadioButtons();

  QString value();
  void setValue(QString value, QString source);

  bool preValid();
  void setWdgValid(bool valid);

  bool setKeyboardFocus();

public slots:
  void childChanged();

protected:
  void changeEvent(QEvent *event);

private:
  QVector < RadioButton* > radiobutton_list;

  QFrame *frame;
};

#endif/*__PRACRO_RADIOBUTTONS_H__*/
