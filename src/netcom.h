/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            netcom.h
 *
 *  Mon Aug 18 09:33:26 CEST 2008
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
#ifndef __PRACRO_NETCOM_H__
#define __PRACRO_NETCOM_H__

#include <QObject>
#include <QString>
//#include <QTcpSocket>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

#include <QWidget>

//#define USE_SSL

//#include "widgets/widget.h"
class Widget;

typedef enum {
  none,
  commit,
  nocommit,
  discard
} session_state_t;

class NetCom : public QObject {
Q_OBJECT
public:
  NetCom(QString host, quint16 port, bool usehttps, bool certverify);
  ~NetCom();

  void send(QString course, QString templ = "", QString macro = "");
  void send(QVector< Widget* > widgets, QString course, QString templ,
            QString macro, QString version);

  void initConnection();
  void commit();
  void nocommit();
  void discard();

  QString sessionid;
  QString user;
  QString patientid;
  QString course;
  QString templ;

signals:
  void gotReply(QDomDocument &doc);
  void networkError(QString title, QString text);

public slots:
  void replyFinished(QNetworkReply*);

private:
  QNetworkAccessManager *manager;
  QString host;
  quint16 port;
  bool certverify = true;
  bool usehttps = true;
  
  //  QString sessionid;
  QWidget *wdg;

  QMap<QNetworkReply *, bool> finished;

  void makeTransfer(QByteArray body, session_state_t state, QString uri = "");
};

#endif/*__PRACRO_NETCOM_H__*/
