/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            collapser.cc
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
#include "collapser.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QPainter>
#include <QLabel>

#include "debug.h"

#define ANIM_TIME 140 // ms
#define ANIM_INTERVAL 20 // ms

Collapser::Collapser(QWidget *current, QScrollArea *scroll)
{
  current_widget = current;
  scrollarea = scroll;

  timer = new QTimer(this);
  timer->setSingleShot(true);
  timer->setInterval(ANIM_INTERVAL);
  connect(timer, &QTimer::timeout, this, &Collapser::anim);

  setLayout(new QHBoxLayout());
  layout()->setContentsMargins(0,0,0,0);

  connect(this, &Collapser::scrollToViewSignal,
          this, &Collapser::scrollToView, Qt::QueuedConnection);
}

void Collapser::updateHeight()
{
  int height = 16;
  if(current_widget) {
    QSize sz = current_widget->minimumSizeHint();
    height = sz.height();
  }
  setFixedHeight(height);
}

QWidget *Collapser::currentWidget()
{
  return current_widget;
}

void Collapser::animateToWidget(QWidget *widget, bool stv)
{
  if(widget == current_widget) return;

  emit animating(widget);

  scroll_to_view = stv;
  t_anim.start();

  placeholder.grabFrom(current_widget);
  if(current_widget) {
    current_widget->setVisible(false);
    layout()->removeWidget(current_widget);
  }

  placeholder.grabTo(widget);
  if(widget) {
    layout()->addWidget(&placeholder);
    placeholder.setVisible(true);
  }

  current_widget = widget;

  timer->start();
}

void Collapser::scrollToView()
{
  if(scroll_to_view && scrollarea && current_widget) {
    scrollarea->ensureWidgetVisible(current_widget);
  }
}

void Collapser::anim()
{
  double x = (double)(t_anim.elapsed()) / (double)ANIM_TIME;
  double y = 1;

  if(x < 1) {
    y = x * x * x;

    placeholder.setWeight(x);

    // Make sure we don't 'bounce' around when animating to the same height.
    int height;
    if(placeholder.fromHeight() == placeholder.toHeight()) {
      height = placeholder.toHeight();
    } else {
      height = (int)((1 - y) * placeholder.fromHeight() +
                        y    * placeholder.toHeight());
    }

    setFixedHeight(height);

    timer->start();

  } else {

    placeholder.setVisible(false);
    layout()->removeWidget(&placeholder);

    if(current_widget) {
      layout()->addWidget(current_widget);
      current_widget->setVisible(true);
    }
    
    setFixedHeight(placeholder.toHeight());

    emit doneAnimating(current_widget);
    emit scrollToViewSignal();
    emit revertTitle();
  }
}

void Collapser::Placeholder::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  p.setOpacity(1 - weight);
  p.drawPixmap(rect(), pixmap_from, pixmap_from.rect());
  p.setOpacity(weight);
  p.drawPixmap(rect(), pixmap_to, pixmap_to.rect());
}

static QPixmap grab(QWidget *w, int &height, int width)
{
  QPixmap pix;
  
  QLabel lbl;

  if(w == nullptr) {
    w = &lbl;
  }

  QSize sz = w->minimumSizeHint();
  sz.setWidth(width);
  w->resize(sz);
  pix = w->grab();

  height = sz.height();

  return pix;
}

void Collapser::Placeholder::grabFrom(QWidget *w)
{
  weight = 0; // Reset

  pixmap_from = ::grab(w, from_height, width());
}

void Collapser::Placeholder::grabTo(QWidget *w)
{
  weight = 0; // Reset

  pixmap_to = ::grab(w, to_height, width());
}

void Collapser::Placeholder::setWeight(double w)
{
  weight = w * w * w;
}

int Collapser::Placeholder::fromHeight()
{
  return from_height;
}

int Collapser::Placeholder::toHeight()
{
  return to_height;
}
