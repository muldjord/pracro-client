/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            launcherwindow.h
 *
 *  Tue Oct 12 11:38:47 CEST 2010
 *  Copyright 2010 Lars Bisballe Jensen
 *  larsbisballe@gmail.com
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
#ifndef __PRACRO_LAUNCHERWINDOW_H__
#define __PRACRO_LAUNCHERWINDOW_H__

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPair>

class LauncherWindow : public QDialog
{
  Q_OBJECT

public:
  LauncherWindow(QString patientId, const QList<QString> &runEntries, QWidget *parent = 0);
  ~LauncherWindow();
  QString getPatientId();
  QString getRunEntry();
  
private slots:
  void checkFields();
  void patientIdChanged(const QString &text);
  
private:
  QLineEdit *patientIdLineEdit;
  QComboBox *runComboBox;
  QString patientIdUndo = "";
  
};

#endif/*__PRACRO_LAUNCHERWINDOW_H__*/
