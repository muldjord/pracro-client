/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            common.cc
 *
 *  Thu Aug 28 15:31:52 CEST 2008
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
#include "common.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDomElement>
#include <QString>

#include <stdio.h>

//#define LINEWIDTH 80

static QString reformatHelpString(QString help)
{
  QString output;

  //  int lastnl = 0;
  for(int i = 0; i < help.size(); i++) {
    if(i < help.size() - 1 && help[i] == '\\' && help[i+1] == 'n') {
      //lastnl = 0;
      output += '\n';
      output += '\n';
      i++;
    } else {
      //if(lastnl > LINEWIDTH && help[i] == ' ') {
      //  output += '\n';
      //  lastnl = 0;
      //} else {
      output += help[i];
      //lastnl++;
      //}
    }
  }
  return output;
}

void setCommonAttributes(QWidget *widget, QDomNode &node)
{
  QDomElement elem = node.toElement();

  if(elem.hasAttribute("width")) {
    widget->setMinimumWidth(elem.attribute("width").toInt());
  }

  if(elem.hasAttribute("height")) {
    widget->setMinimumHeight(elem.attribute("height").toInt());
  }

  if(elem.hasAttribute("help")) {
    QString helptext = reformatHelpString(elem.attribute("help"));
    widget->setWhatsThis(helptext);
    //    widget->setToolTip(helptext);
  }

  if(elem.hasAttribute("colour") &&
     elem.attribute("colour").length() == 6) {
    unsigned int r, g, b;
    sscanf(elem.attribute("colour").toStdString().c_str(),
           "%02x%02x%02x", &r, &g, &b);

    char style[128];
    sprintf(style, "* { color: #%02x%02x%02x; }",
            r, g, b);
    widget->setStyleSheet(style);
  }

  if(elem.hasAttribute("bgcolour") &&
     elem.attribute("bgcolour").length() == 6) {
    unsigned int r, g, b;
    sscanf(elem.attribute("bgcolour").toStdString().c_str(),
           "%02x%02x%02x", &r, &g, &b);

    char style[128];
    sprintf(style, "* { background-color: #%02x%02x%02x; }",
            r, g, b);
    widget->setStyleSheet(style);
  }

}

void setCommonLayout(QWidget *widget, QDomNode &node)
{
  QDomElement elem = node.toElement();

  if(elem.attribute("layout", "vbox") == "hbox") {
    widget->setLayout(new QHBoxLayout(widget));
  } else {
    widget->setLayout(new QVBoxLayout(widget));
  }

  widget->setContentsMargins(0,0,0,0);
  //widget->layout()->setContentsMargins(0,0,0,0);
}

bool isUserSource(QString source)
{
  if(source == "pentominos") return true;
  if(source == "pracro") return false;
  return true;
}
