/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            window.cc
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
#include "window.h"
#include "common.h"

#include <QWidget>
#include <QIcon>

Window::Window(QDomNode &node, MacroWindow *macrowindow)
  : Widget(node, macrowindow)
{
  setWidget(new QWidget(nullptr));

  getWidget()->setWindowFlags(Qt::WindowContextHelpButtonHint |
                         Qt::WindowSystemMenuHint);

  getWidget()->setWindowIcon(QIcon(":icon.png"));

  setCommonAttributes(getWidget(), node);
  setCommonLayout(getWidget(), node);

  QDomElement elem = node.toElement();

  if(elem.hasAttribute("fixed")) {
    if(elem.attribute("fixed") == "true") {
      getWidget()->setFixedSize(getWidget()->width(), getWidget()->height());
    }
  }

  if(elem.hasAttribute("caption")) {
    getWidget()->setWindowTitle(elem.attribute("caption"));
  }

  addChildren(node, getWidget()->layout());
}

Window::~Window()
{
  //delete widget;
}

bool Window::setKeyboardFocus()
{
  return false;
}
