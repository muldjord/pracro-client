/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            datetime.cc
 *
 *  Fri Jan  9 09:00:12 CET 2009
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
#include "datetime.h"

#include <QDateTimeEdit>

#include "common.h"

DateTime::DateTime(QDomNode &node, MacroWindow *macrowindow)
  : Widget(node, macrowindow)
{
  datetimeedit = new QDateTimeEdit();
  setWidget(datetimeedit);

  changedByUser = true;
  setCommonAttributes(datetimeedit, node);

  datetimeedit->setCalendarPopup(true);
//  setMinimumDateTime(QDateTime::fromTime_t(0));
  datetimeedit->setMinimumDate(QDate(1900,1,1));
  datetimeedit->setMinimumTime(QTime(0,0));

  QDomElement elem = node.toElement();

  switch(elem.attribute("fuzziness", "5").toLong()) {
  case 1:
    datetimeedit->setDisplayFormat("yyyy");
    break;

  case 2:
    datetimeedit->setDisplayFormat("MMMM yyyy");
    break;

  case 3:
    datetimeedit->setDisplayFormat("dd MMMM yyyy");
    break;

  case 4:
    datetimeedit->setDisplayFormat("dd MMMM yyyy hh");
    break;

  case 5:
    datetimeedit->setDisplayFormat("dd MMMM yyyy hh:mm");
    break;

  case 6:
    datetimeedit->setDisplayFormat("dd MMMM yyyy hh:mm:ss");
    break;

  case 7:
  default:
    datetimeedit->setDisplayFormat("dd MMMM yyyy hh:mm:ss:zzz");
    break;
  }
    
  /*
  if(elem.hasAttribute("readonly")) {
    if(elem.attribute("readonly") == "true" ||
       elem.attribute("readonly") == "1") {
      setReadOnly(true);
    } else if(elem.attribute("readonly") == "false" ||
              elem.attribute("readonly") == "0") {
      setReadOnly(false);
    } else {
      printf("Unknown value of readonly: %s\n",
             elem.attribute("readonly").toStdString().c_str());
    }
  }
  */
  connect(datetimeedit, &QDateTimeEdit::dateTimeChanged, this, &DateTime::changed);
}

DateTime::~DateTime()
{
  // delete datetimeedit;
}

void DateTime::changed(const QDateTime &)
{
  emit eventOnChange();
  if(changedByUser) emit wasChanged(); 
}

QString DateTime::value()
{
  return QString::number(datetimeedit->dateTime().toUTC().toTime_t());
}

void DateTime::setValue(QString value, QString source)
{
  changedByUser = false;
  if(isUserSource(source)) emit wasChanged();

  datetimeedit->setDateTime(QDateTime::fromTime_t(value.toUInt()));

  //  setInitialValue(value);
  changedByUser = true;
}

void DateTime::setWdgValid(bool valid)
{
  QPalette palette;

  if(valid) {
    // valid string
    palette.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255)));
  } else {
    // invalid string
    palette.setBrush(QPalette::Base, QBrush(QColor(200, 230, 200)));
  }

  datetimeedit->setPalette(palette);
}
