/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            aboutbox.h
 *
 *  Tue Oct 11 14:39:02 CEST 2011
 *  Copyright 2011 Lars Bisballe Jensen
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
#ifndef __PRACRO_ABOUTBOX_H__
#define __PRACRO_ABOUTBOX_H__

#include <QDialog>
#include <QMouseEvent>
#include <QScrollArea>
#include <QTabWidget>

class AboutBox : public QDialog
{
Q_OBJECT

public:
  AboutBox(QWidget *parent, Qt::WindowFlags f);

public slots:
  void scroll();
  void noMoreScroll();
  void checkTab(int tabIndex);

signals:

private:
  void mousePressEvent(QMouseEvent * event);
  QScrollArea *licenseScroll;
  QTimer *scrollTimer;
  int scrollState;
  QTabWidget *tabs;
};


#endif/*__PRACRO_ABOUTBOX_H__*/
