/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            label.cc
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
#include "label.h"
#include <stdio.h>

#include <QLabel>

#include "common.h"

Label::Label(QDomNode &node, MacroWindow *macrowindow)
  : Widget(node, macrowindow)
{
  label = new QLabel();
  setWidget(label);

  QDomElement elem = node.toElement();

  setCommonAttributes(label, node);

  label->setWordWrap(true);

  if(elem.hasAttribute("caption")) {
    label->setText(elem.attribute("caption"));
  } else {
    label->setText(elem.attribute(""));
  }

  // Check for horizontal alignment and set it up accordingly
  if(elem.hasAttribute("alignment")) {
    if(elem.attribute("alignment") == "left") {
      label->setAlignment(Qt::AlignLeft);
    } else if (elem.attribute("alignment") == "center") {
      label->setAlignment(Qt::AlignHCenter);
    } else if (elem.attribute("alignment") == "right") {
      label->setAlignment(Qt::AlignRight);
    }
  } else {
    label->setAlignment(Qt::AlignLeft);
  }
  
  // Always center vertically in the addressed space
  label->setAlignment(Qt::AlignVCenter);
}

Label::~Label()
{
  // delete label;
}
