/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            metawidget.h
 *
 *  Wed Nov 26 08:51:52 CET 2008
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
#ifndef __PRACRO_METAWIDGET_H__
#define __PRACRO_METAWIDGET_H__

#include "widget.h"

#include <QDomNode>

#include <QListWidget>
#include <QVector>

/***
 * Meta Widget
 * @clientside
 * @extends widget
 * @tag metawidget
 * @screenshot An example of a metawidget with two inner widgets. The metawidget is not in itself visible.
 * <metawidget layout="vbox" name="dims" format="''" storechildren="true">
 *   <lineedit name="test1" value="test"/>
 *   <checkbox name="test2" value="ja" truevalue="ja" falsevalue="nej"
 *             caption="Og svaret er?"/>
 * </metawidget>
 * @container
 * This widget is simply a container compositing its value from the widgets
 * within it. The value of the widget cannot be set and its value attribute is
 * therefore ignored.
 * @example An example of how to use metawidget with two inner widgets:
 * <metawidget layout="vbox" name="dims" storechildren="true"
 *             format="return widget('test1'):value() .. ': ' .. widget('test2'):value()">
 *   <lineedit name="test1" value="test"/>
 *   <checkbox name="test2" value="ja" truevalue="ja" falsevalue="nej"
 *             caption="Og svaret er?"/>
 * </metawidget>
 * @att layout the layout of the widget. Can be one of 'vbox' or 'hbox'.
 * @att storechildren This attribute indicates wether the child widgets should
 * be stored on macro commit. It can be either 'true' or 'false'.
 * @att format This attribute contains lua code. The return value of the code
 * is the resulting value of the meta widget.
 * @att layout The layout assigned to the widget. Can be one of 'hbox' or
 * 'vbox'.
 */

class QFrame;
class MetaWidget : public Widget
{
Q_OBJECT
public:
  MetaWidget(QDomNode &node, MacroWindow *macrowindow);
  ~MetaWidget();

  QString value();
  void setValue(QString value, QString source);

  bool preValid();
  void setWdgValid(bool valid);

  bool setKeyboardFocus();

  //  void reset();

public slots:
  void changed();

private:
  QListWidget *list;
  QVector< Widget* > widgets;
  QString format;
  QString formatlanguage;
  bool storechildren;

  QFrame *frame;
};

#endif/*__PRACRO_METAWIDGET_H__*/
