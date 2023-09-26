/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            template.cc
 *
 *  Fri Jul  1 14:43:46 CEST 2011
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
#include "template.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>

Template::Template(NetCom &n)
  : netcom(n)
{
  wdg = new QGroupBox();

  QVBoxLayout *l = new QVBoxLayout();
  wdg->setLayout(l);
  wdg->setContentsMargins(50, 0, 0, 0);

  l->addStretch();

  templ = new QLabel();
  QFont font = templ->font();
  font.setBold(true);
  font.setPointSize(font.pointSize() * 1.5);
  templ->setFont(font);
  l->addWidget(templ);

  l->addSpacing(10);

  QPushButton *btn = new QPushButton(tr("Open"));
  btn->setStyleSheet(":focus{ background-color: #2199e8; }"
                     ":hover{ background-color: #2199e8; }");
  btn->setIcon(QIcon(":open.png"));
  btn->setFixedSize(200,48);
  l->addWidget(btn);
  connect(btn, &QPushButton::clicked, this, &Template::open);

  l->addStretch();

}

const char *Template::type()
{
  return "template";
}

void Template::update(QDomNode &node)
{
  QDomElement elem = node.toElement();
  templ->setText(elem.attribute("title") + " (" + elem.attribute("name") + ")");
  name = elem.attribute("name");
}

void Template::open()
{
  emit setTemplate(name);
  netcom.templ = name;
  netcom.send(netcom.course, name, "");
}

QWidget *Template::widget()
{
  return wdg;
}

#ifdef TEST_TEMPLATE
//Additional dependency files
//deps:
//Required cflags (autoconf vars may be used)
//cflags:
//Required link options (autoconf vars may be used)
//libs:
#include "test.h"

TEST_BEGIN;

// TODO: Put some testcode here (see test.h for usable macros).
TEST_TRUE(false, "No tests yet!");

TEST_END;

#endif/*TEST_TEMPLATE*/
