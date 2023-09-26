/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            multilist.h
 *
 *  Mon Jun 16 15:31:24 CEST 2008
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
#ifndef __PRACRO_MULTILIST_H__
#define __PRACRO_MULTILIST_H__

#include "widget.h"

#include <QDomNode>

#include <QListWidget>
#include <QVector>

/***
 * Multi List Widget
 * @tag multilist
 * @clientside
 * @container
 * @screenshot Example of multilist containing a metawidget.
 * <multilist name="x" layout="vbox" innerwidget="in" value="Abc: Def
 * Hij: Klm">
 *   <metawidget name="in" layout="hbox" format="return ''">
 *     <lineedit name="a" value="Abc"/>
 *     <label caption=":"/>
 *     <lineedit name="b" value="Def"/>
 *   </metawidget>
 * </multilist>
 * @extends widget
 * This widget is used to produce a list of text items constructed using other
 * widgets. The value of the widget is contained in a simple string using the
 * newline character to seperate items.
 * @att layout the layout of the widget. Can be one of 'vbox' or 'hbox'.
 * @att innerwidget This attribute contains the name of the child widget used
 * when producing a new value for the list. In order to use multiple widgets see
 * the @see metawidget.
 */

class QFrame;
class MultiList : public Widget
{
Q_OBJECT
public:
  MultiList(QDomNode &node, MacroWindow *macrowindow);
  ~MultiList();

  QString value();
  void setValue(QString value, QString source = "");

  void setWdgValid(bool valid);

  bool setKeyboardFocus();

  void setValues();

public slots:
  void changed();
  void remove();
  void add();

protected:
  bool eventFilter(QObject *obj, QEvent *event);

  // Implement in subclasses to be able to block commits.
  bool preCommit();

  // Don't connect children 'wasChanged' signal to parent.
  void connectChild(Widget *) {}

private:
  QListWidget *list;
  Widget *innerwidget;
  QString format;
  bool innerwidget_has_changes;

  QFrame *frame;
};

#endif/*__PRACRO_MULTILIST_H__*/
