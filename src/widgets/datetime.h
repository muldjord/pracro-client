/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            datetime.h
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
#ifndef __PRACRO_DATETIME_H__
#define __PRACRO_DATETIME_H__

#include "widget.h"

#include <QDomNode>
#include <QDateTime>

/***
 * Date and Time Widget
 * @tag datetime
 * @clientside
 * @screenshot Example with fuzziness="1" and value="1234567890"
 * <datetime name="x" fuzziness="1" value="1234567890"/>
 * @screenshot Example with fuzziness="7" and value="1234567890"
 * <datetime name="x" fuzziness="7" value="1234567890"/>
 * @extends widget
 * This widget is used to get structured time and date information.
 * <em>NOTE</em>: The value attribute must be a unix timestamp (number of
 * seconds since epoch: 1/1-1970).
 * @att fuzziness The precision of the display. Values from 1 - 7 can be used
 * with the following meaning:
 * <ul>
 *   <li>1: yyyy</li>
 *   <li>2: MMMM yyyy</li>
 *   <li>3: dd MMMM yyyy</li>
 *   <li>4: dd MMMM yyyy hh</li>
 *   <li>5: dd MMMM yyyy hh:mm</li>
 *   <li>6: dd MMMM yyyy hh:mm:ss</li>
 *   <li>7: dd MMMM yyyy hh:mm:ss:zzz</li>
 * </ul>
 */
class QDateTimeEdit;
class DateTime : public Widget
{
Q_OBJECT
public:
  DateTime(QDomNode &node, MacroWindow *macrowindow);
  ~DateTime();

  QString value();
  void setValue(QString value, QString source = "");

  void setWdgValid(bool valid);

public slots:
  void changed(const QDateTime &);

private:
  bool changedByUser;
  QDateTimeEdit *datetimeedit;
};

#endif/*__PRACRO_DATETIME_H__*/
