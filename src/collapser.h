/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            collapser.h
 *
 *  Mon Sep 15 09:48:46 CEST 2008
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
#ifndef __PRACRO_COLLAPSER_H__
#define __PRACRO_COLLAPSER_H__

#include <QWidget>
#include <QElapsedTimer>
#include <QTimer>

#include <QScrollArea>
#include <QPixmap>

class Collapser : public QWidget {
Q_OBJECT
public:
  Collapser(QWidget *current = nullptr, QScrollArea *scrollarea = nullptr);

  void updateHeight();

  void animateToWidget(QWidget *widget, bool scroll_to_view);

  QWidget *currentWidget();

public slots:
  void anim();
  void scrollToView();

signals:
  void animating(QWidget *);
  void doneAnimating(QWidget *);
  void scrollToViewSignal();
  void setBusyTitle();
  void revertTitle();

protected:
  //  void timerEvent(QTimerEvent *);

private:
  class Placeholder : public QWidget {
  public:
    void grabFrom(QWidget *w);
    void grabTo(QWidget *w);

    void setWeight(double weight);

    int toHeight();
    int fromHeight();

  protected:
    void paintEvent(QPaintEvent*);

  private:
    QPixmap pixmap_from;
    QPixmap pixmap_to;

    int to_height;
    int from_height;
    double weight;
  };

  Placeholder placeholder;
  QWidget *current_widget;

  bool scroll_to_view;
  QTimer *timer;
  QElapsedTimer t_anim;
  int timer_id;

  QScrollArea *scrollarea;
};

#endif/*__PRACRO_COLLAPSER_H__*/
