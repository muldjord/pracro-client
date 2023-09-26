/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            netcom.cc
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
#include "netcom.h"
#include "debug.h"
#include "widgets/widget.h"

#include <QtNetwork>
#include <QApplication>
#include <QByteArray>
#include <QWidget>

#ifdef QT_NO_OPENSSL
#error "QT not compiled with SSL support."
#endif

NetCom::NetCom(QString host, quint16 port, bool usehttps, bool certverify)
{
  this->host = host;
  this->port = port;
  this->certverify = certverify;
  this->usehttps = usehttps;

  manager = new QNetworkAccessManager(this);
  connect(manager, &QNetworkAccessManager::finished, this, &NetCom::replyFinished);
}

NetCom::~NetCom()
{
  delete manager;
}

void NetCom::replyFinished(QNetworkReply *reply)
{
  if(reply->error() == QNetworkReply::NoError) {
    if(reply->hasRawHeader("SessionID")) {
      sessionid = reply->rawHeader("SessionID");
      LOG(netcom, "Reply SESSION ID: %s\n", sessionid.toStdString().c_str());
    }

    QDomDocument doc;
    doc.setContent(reply->readAll());
    emit gotReply(doc);

  } else {
    emit networkError("Network Error", reply->errorString());
  }
  reply->deleteLater();
}

void NetCom::makeTransfer(QByteArray body, session_state_t state, QString uri)
{
  QNetworkRequest request;

  QUrl url;
  url.setHost(host);
  url.setPort(port);
  if(usehttps) {
    url.setScheme("https");
    if(!certverify) {
      QSslConfiguration conf = request.sslConfiguration();
      conf.setPeerVerifyMode(QSslSocket::VerifyNone);
      request.setSslConfiguration(conf);
    }
  } else {
    url.setScheme("http");
  }
  
  request.setUrl(url);
  // Use '/course/template/' uri if empty.
  if(uri == "") {
    uri = "/";
    if(course != "") {
      uri += course + "/";
      if(templ != "") {
        uri += templ + "/";
      }
    }
  }

  DEBUG(netcom, "Making transfer:\n%s", QString(body).toStdString().c_str());

  LOG(netcom, "SESSION ID: %s\n", sessionid.toStdString().c_str());

  request.setHeader(QNetworkRequest::UserAgentHeader, "Pracro Client v" VERSION);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  /*
  if(sessionid != "") {
    request.setRawHeader("SessionID", sessionid.toStdString().c_str());
  } else {
    request.setRawHeader("SessionPatientID", patientid.toStdString().c_str());
    request.setRawHeader("SessionTemplate", templ.toStdString().c_str());
  }
  */

	QUrlQuery query;

  switch(state) {
  case ::commit:
    query.addQueryItem("statechange", "commit");
    break;
  case ::discard:
    query.addQueryItem("statechange", "discard");
    break;
  case ::nocommit:
    query.addQueryItem("statechange", "nocommit");
    break;
  default:
  case ::none:
    break;
  }

  url.setPath(uri);
  if(sessionid != "") query.addQueryItem("sessionid", sessionid);
  if(patientid != "") query.addQueryItem("patientid", patientid);

	url.setQuery(query);

  request.setUrl(url);

  manager->post(request, body);
}

void NetCom::initConnection()
{
  send(course, templ); // Initialise by sending a template-only request.
}

void NetCom::commit()
{
  makeTransfer("", ::commit);
}

void NetCom::nocommit()
{
  makeTransfer("", ::nocommit);
}

void NetCom::discard()
{
  makeTransfer("", ::discard);
}

void NetCom::send(QString course, QString templ, QString macro)
{
  /*
  QDomProcessingInstruction header =
    doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
  doc.appendChild(header); 

  QDomElement pracro_elem = doc.createElement("pracro");
  pracro_elem.setAttribute("version", "1.0");
  pracro_elem.setAttribute("patientid", patientid);
  pracro_elem.setAttribute("user", user);
  doc.appendChild(pracro_elem);

  QDomElement request_elem = doc.createElement("request");
  request_elem.setAttribute("course", course);
  if(templ != "") request_elem.setAttribute("template", templ);
  if(macro != "") request_elem.setAttribute("macro", macro);
  pracro_elem.appendChild(request_elem);
  */
  QString uri = "/";
  if(course != "") {
    uri += course + "/";
    if(templ != "") {
      uri += templ + "/";
      if(macro != "") uri += macro + "/";
    }
  }

  makeTransfer("", ::none, uri);
}

void NetCom::send(QVector< Widget* > widgets, QString course, QString templ,
                  QString macro, QString version)
{
  QDomDocument doc;

  QDomProcessingInstruction header =
    doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
  doc.appendChild(header); 

  QDomElement pracro_elem = doc.createElement("pracro");
  pracro_elem.setAttribute("version", "1.0");
  pracro_elem.setAttribute("patientid", patientid);
  pracro_elem.setAttribute("user", user);
  doc.appendChild(pracro_elem);

  QDomElement commit_elem = doc.createElement("commit");
  commit_elem.setAttribute("course", course);
  commit_elem.setAttribute("template", templ);
  commit_elem.setAttribute("macro", macro);
  commit_elem.setAttribute("version", version);
  pracro_elem.appendChild(commit_elem);

  // Iterate the different entries, and append their results to
  //  the commit string
  QVector< Widget* >::iterator i = widgets.begin();
  while(i != widgets.end()) {
    DEBUG(netcom, "W = ");
    if(*i) {
      Widget* w = *i;
      DEBUG(netcom, "name: %s val: %s", w->name().toStdString().c_str(),
            w->value().toStdString().c_str());
      if(w->enabled() && w->name() != "" && w->local() == false) {
        QDomElement field_elem = doc.createElement("field");
        field_elem.setAttribute("name", w->name());
        field_elem.setAttribute("value", w->value());
        commit_elem.appendChild(field_elem);
      }
    }
    i++;
  }
  /*
  QDomElement request_elem = doc.createElement("request");
  request_elem.setAttribute("template", templ);
  pracro_elem.appendChild(request_elem);
  */
  makeTransfer(doc.toByteArray(), ::none, "");
}
