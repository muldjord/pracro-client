/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            resumewidget.h
 *
 *  Tue Sep 23 12:02:03 CEST 2008
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
#ifndef __PRACRO_RESUMEWIDGET_H__
#define __PRACRO_RESUMEWIDGET_H__

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

class ResumeWidget : public QWidget {
Q_OBJECT
public:
  ResumeWidget(bool compact);

  typedef enum {
    NEW,
    OLD,
    DIRTY
  } state_t;

  void setText(QString text, state_t state);
  QString getText();
                                           
public slots:
  void showFull(const QString &);  

private:
  QString fulltext;
  QLabel *resume;
  bool compact;
};

#endif/*__PRACRO_RESUMEWIDGET_H__*/
