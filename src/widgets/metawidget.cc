/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            metawidget.cc
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
#include "metawidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>

#include "messagebox.h"
#include "macrowindow.h"
#include "common.h"

#include "debug.h"

MetaWidget::MetaWidget(QDomNode &node, MacroWindow *macrowindow)
  : Widget(node, macrowindow)
{
  frame = new QFrame();
  setWidget(frame);

  setCommonAttributes(frame, node);
  setCommonLayout(frame, node);

  hideChildren = true;
  
  QDomElement elem = node.toElement();
  storechildren = elem.attribute("storechildren", "false") == "true";

  format = elem.attribute("format", "");

  addChildren(node, frame->layout());

  // Connect all children wasChanged signal, to catch changes.
  for(auto *w: widgets) {
    connect(w, &Widget::wasChanged, this, &MetaWidget::changed);
  }
}

MetaWidget::~MetaWidget()
{
  // delete frame;
}

void MetaWidget::changed()
{
  emit wasChanged();
}

QString MetaWidget::value()
{
  return lua->runScriptS(format, this, "format");
}

void MetaWidget::setValue(QString, QString)
{
  WARN(metawidget, "setValue was attempted on this widget.");
}

bool MetaWidget::preValid()
{
  // If children are stored they will validate themselves.
  if(!storechildren) {
    for(auto *w: widgets) {
      if(w->enabled() && !w->local() && !w->valid()) {
        MessageBox::critical(nullptr, "Fejl",
                             "Et af inputfelterne (" + w->name()
                             + ") er ikke udfyldt korrekt, prøv igen.\n",
                             MessageBox::Ok);
        return false;
      }
    }
  }

  return true;
}

bool MetaWidget::setKeyboardFocus()
{
  for(auto *w: widgets) {
    if(w->setKeyboardFocus()) return true;
  }
  return false;
}

void MetaWidget::setWdgValid(bool valid)
{
  QPalette palette;

  if(valid) {
    // valid string
    palette.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255)));
  } else {
    // invalid string
    palette.setBrush(QPalette::Base, QBrush(QColor(230, 200, 200)));
  }

  frame->setPalette(palette);
}
