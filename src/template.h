/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            template.h
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
#ifndef __PRACRO_TEMPLATE_H__
#define __PRACRO_TEMPLATE_H__

#include <QLabel>

#include "entity.h"
#include "netcom.h"

class Template : public QObject, public Entity {
Q_OBJECT
public:
  Template(NetCom &netcom);
  const char *type();
  void update(QDomNode &node);
  QWidget *widget();

public slots:
  void open();

signals:
  void setTemplate(QString name);

private:
  QString name;
  QLabel *templ;
  QWidget *wdg;
  NetCom &netcom;
};

#endif/*__PRACRO_TEMPLATE_H__*/
