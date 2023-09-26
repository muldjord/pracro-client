/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            frame.cc
 *
 *  Wed Jul 18 09:56:37 CEST 2007
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
#include "frame.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

#include "common.h"

Frame::Frame(QDomNode &node, MacroWindow *macrowindow)
  : Widget(node, macrowindow)
{
  frame = new QFrame();
  setWidget(frame);

  setCommonAttributes(frame, node);
  setCommonLayout(frame, node);

  frame->layout()->setContentsMargins(0,0,0,0);

  addChildren(node, frame->layout());
}

Frame::~Frame()
{
  // delete frame;
}

bool Frame::setKeyboardFocus()
{
  QVector< Widget* >::iterator i = children.begin();
  while(i != children.end()) {
    Widget *w = *i;
    if(w) {
      if(w->setKeyboardFocus()) return true;
    }
    i++;
  }

  return false;
}
