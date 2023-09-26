/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            shortcuttooltip.cc
 *
 *  Thu Dec 20 14:17:03 CET 2012
 *  Copyright 2012 Bent Bisballe Nyeng
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
#include "shortcuttooltip.h"

#include <QPalette>

ShortcutTooltip::ShortcutTooltip(QString label, QWidget *parent)
  : QLabel(label, parent)
{
  resize(minimumSizeHint());
    //    shortcut_label->setWindowFlags(Qt::ToolTip);
  QFont f = font();
  f.setPixelSize(9);
  setFont(f);

  setAutoFillBackground(true);
  //    shortcut_label->setMargin(4);

  alpha = 0;

  QPalette pal = palette();
  pal.setColor(foregroundRole(), QColor(0,0,0, alpha));
  pal.setColor(backgroundRole(), QColor(255,255,170,alpha));
  setPalette(pal);

    //shortcut_label->move(0, 0);
  setVisible(false);

  connect(&timer, &QTimer::timeout, this, &ShortcutTooltip::timeout);
  timer.setSingleShot(true);
}

void ShortcutTooltip::setShow(bool s)
{
  if(s) {
    modifier = 0.3;
    raise();
    setVisible(true);
    timer.start(25);
  } else {
    modifier = -0.4;
    timer.start(25);
  }
}

void ShortcutTooltip::timeout()
{
  alpha += modifier;

  if(modifier > 0) {

    if(alpha > 1.0) {
      alpha = 1.0;
    } else {
      timer.start(25);
    }

  } else {

    if(alpha < 0.0) {
      setVisible(false);
      alpha = 0;
    } else {
      timer.start(25);
    }

  }

  QPalette pal = palette();
  pal.setColor(foregroundRole(), QColor(0,0,0, 255 * alpha));
  pal.setColor(backgroundRole(), QColor(255,255,170, 150 * alpha));
  setPalette(pal);
}
