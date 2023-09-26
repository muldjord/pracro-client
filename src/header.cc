/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            header.cc
 *
 *  Fri Mar  4 15:02:40 CET 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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
#include "header.h"

#include <QFont>

Header::Header()
{
  header = new QLabel();
  QFont headerfont = header->font();
  headerfont.setBold(true);
  headerfont.setPointSize(headerfont.pointSize() + 2);
  header->setFont(headerfont);
}

const char *Header::type()
{
  return "header";
}

void Header::update(QDomNode &node)
{
  QDomElement elem = node.toElement();

  if(elem.tagName() != "header") return;

  header->setText(elem.attribute("caption"));
}


QWidget *Header::widget()
{
  return header;
}
