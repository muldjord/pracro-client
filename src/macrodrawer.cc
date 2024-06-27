/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            macrodrawer.cc
 *
 *  Tue Jan  6 15:49:48 CET 2009
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
#include "macrodrawer.h"

#include <QPushButton>
#include <QApplication>
//#include <QSvgRenderer>
#include <QPainter>
#include <QImage>
#include <QEvent>
#include <QCloseEvent>
#include <QMouseEvent>

#include "macro.h"

MacroDrawer::MacroDrawer(Macro *macro, QString title)
{
  button = nullptr;

  busyTitle = tr("       Processing, please wait...");
  macroTitle = "       " + title;
  if(!macro->isstatic) setTitle(macroTitle);
  setFlat(true);
  
  {
    QFont f = font();
    f.setItalic(true);
    if(macro->isimportant) {
      f.setBold(true);
    }
    setFont(f);
  }
  
  if(!macro->isstatic) {
    button = new ExpandButton(this);
    button->setFixedSize(16,16);
    button->move(0,0);
    {
      QFont f = button->font();
      f.setBold(false);
      f.setItalic(false);
      button->setFont(f);
    }

    installEventFilter(this);
    connect(button, &ExpandButton::clicked, this, &MacroDrawer::toggleMe);
  }

  //int l,t,r,b;
  //getContentsMargins(&l,&t,&r,&b);
  //l = 6;
  //setContentsMargins(l, t, r, b);
  QMargins margins = contentsMargins();
  setContentsMargins(6, margins.top(), margins.right(), margins.bottom());
}

MacroDrawer::~MacroDrawer()
{
}

bool MacroDrawer::eventFilter(QObject *obj, QEvent *event)
{
  if(event->type() == QEvent::MouseButtonRelease) {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

    QFontMetrics fontMetrics(font());
    // Make sure we can also open the macro by clicking the title
    if(mouseEvent->y() < fontMetrics.height() && isEnabled() &&
       mouseEvent->x() < fontMetrics.horizontalAdvance(title()))
      toggleMe();
  }
  return QObject::eventFilter(obj, event);
}

void MacroDrawer::toggleMe()
{
  setBusyTitle();
  emit toggle();
}

void MacroDrawer::activationChange(bool active)
{
  //setEnabled(active);

  if(!active) {
    // Set padlock icon on button.
    if(button) {
      button->setIcon(QPixmap(":padlock.png"));
    }

  } else {
    // Clear padlock icon on button
    if(button) {
      button->setIcon(QPixmap());
    }
  }
}

void MacroDrawer::setBusyTitle()
{
  setTitle(busyTitle);
}

void MacroDrawer::revertTitle()
{
  setTitle(macroTitle);
}
