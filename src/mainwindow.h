/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mainwindow.h
 *
 *  Wed Sep 17 09:41:08 CEST 2008
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
#ifndef __PRACRO_MAINWINDOW_H__
#define __PRACRO_MAINWINDOW_H__

#include <QMainWindow>
#include <QMap>
#include <QPushButton>
#include <QFont>
#include <QLabel>
#include <QDomDocument>
#include <QScrollArea>
#include <QTimer>

#include "netcom.h"
#include "entity.h"

class MainWindow : public QMainWindow {
Q_OBJECT
public:
  MainWindow(QString patientid, QString title, QString course,
             QString host, quint16 port, bool usehttps, bool certverify, QString user);
  ~MainWindow();

  void closeEvent(QCloseEvent *event);

signals:
  void isClosing();

public slots:
  //  void update();
  void showAbout();
  void closeCommit();
  void closeNoCommit();
  void closeDiscard();

  void handle(QDomDocument &doc);
  void showError(QString title, QString text);

  bool hasOpen(void *me);
  void setTemplate(QString name);

private:
  void copyToClipBoard();
  void enableEntities(bool enable);
  void updateTemplates(QDomNodeList &nodes);
  void updateMacros(QDomNodeList &nodes);
  void updateTemplateHeaders(QDomNode &templatenode);

  QLabel *patientid_label;
  QString patientTitle;

  QString course;
  QString templ;

  NetCom netcom;

  QString patientid;
  QString user;

  QString configFile = "./configs/_generic.ini";
  
  QWidget *central;

  Entities entities;

  QLabel *header;

  QPushButton *aboutButton;

  bool initialising;

  //  Sessions sessions;
  QScrollArea *scrollarea;

  bool closing;

  bool isStored;

  QTimer focusTimer;
  bool lostFocus = false;
               
private slots:
  void init();
  void checkFocus();
  
};

#endif/*__PRACRO_MAINWINDOW_H__*/
