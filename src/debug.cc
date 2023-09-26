/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            debug.cc
 *
 *  Tue Aug 17 10:48:45 CEST 2010
 *  Copyright 2010 Bent Bisballe Nyeng
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
#include "debug.h"

#include <QVBoxLayout>
#include <QCloseEvent>
#include <QSettings>
#include <QAbstractItemView>
#include <QToolBar>
#include <QAction>
#include <QWidgetAction>
#include <QLabel>
#include <QLineEdit>
#include <QTextStream>

#include <stdio.h>

static const char * const class_str[] =
  { "debug", "error", "warn", "log" };

DebugWindow::DebugWindow()
{
  QWidget *center = new QWidget();
  setCentralWidget(center);

  center->setLayout(new QVBoxLayout());
  lst = new QListWidget();
  lst->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  center->layout()->addWidget(lst);

  QToolBar *toolbar = addToolBar("Tools");
  toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  QAction *clear = toolbar->addAction("Clear");
  connect(clear, &QAction::triggered, this, &DebugWindow::clear);

  QToolBar *toolbar2 = addToolBar("Filters");
  toolbar2->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  QAction *a = new QAction(toolbar2);
  a->setText("debug");
  a->setCheckable(true);
  toolbar2->addAction(a);
  connect(a, &QAction::toggled, this, &DebugWindow::debug_toggled);
  a->setChecked(true);

  a = new QAction(toolbar2);
  a->setText("error");
  a->setCheckable(true);
  toolbar2->addAction(a);
  connect(a, &QAction::toggled, this, &DebugWindow::error_toggled);
  a->setChecked(true);

  a = new QAction(toolbar2);
  a->setText("warning");
  a->setCheckable(true);
  toolbar2->addAction(a);
  connect(a, &QAction::toggled, this, &DebugWindow::warning_toggled);
  a->setChecked(true);

  a = new QAction(toolbar2);
  a->setText("log");
  a->setCheckable(true);
  toolbar2->addAction(a);
  connect(a, &QAction::toggled, this, &DebugWindow::log_toggled);
  a->setChecked(true);

  QWidgetAction *wa = new QWidgetAction(toolbar2);
  wa->setDefaultWidget(new QLabel("Filter:"));
  toolbar2->addAction(wa);

  wa = new QWidgetAction(toolbar2);
  QLineEdit *fe = new QLineEdit();
  wa->setDefaultWidget(fe);
  toolbar2->addAction(wa);
  connect(fe, &QLineEdit::textChanged, this, &DebugWindow::filter_changed);

  QSettings settings("Aasimon.org", "Pracro");
  settings.beginGroup("DebugWindow");
  resize(settings.value("size", QSize(700, 800)).toSize());
  move(settings.value("pos", QPoint(0, 0)).toPoint());
  settings.endGroup();

  timer.setInterval(300);
  timer.setSingleShot(true);
  connect(&timer, &QTimer::timeout, lst, &QListWidget::scrollToBottom);
}

void DebugWindow::closeEvent(QCloseEvent *event)
{
  QSettings settings("Aasimon.org", "Pracro");
  settings.beginGroup("DebugWindow");
  settings.setValue("size", size());
  settings.setValue("pos", pos());
  settings.endGroup();
  event->accept();
}

void DebugWindow::clear()
{
  lst->clear();
}

void DebugWindow::filter_changed(QString f)
{
  filter = f;
}

void DebugWindow::debug_toggled(bool on)
{
  show_debug = on;
}

void DebugWindow::error_toggled(bool on)
{
  show_error = on;
}

void DebugWindow::warning_toggled(bool on)
{
  show_warning = on;
}

void DebugWindow::log_toggled(bool on)
{
  show_log = on;
}

void DebugWindow::log(const char *func, const char *file, const int line,
                      debug_class cl, const char *ch, QString &msg)
{
  if(cl == _debug && show_debug == false) return; // Ignore
  if(cl == _error && show_error == false) return; // Ignore
  if(cl == _warn && show_warning == false) return; // Ignore
  if(cl == _log && show_log == false) return; // Ignore

  // Remove trailing newlines.
  while(msg.endsWith("\n")) msg = msg.left(msg.length() - 1);

  ch = ch;
  QString txt;
  QTextStream(&txt) << "[" << class_str[(unsigned)cl] << "] "
                    << file << ":"
                    << func << ":"
                    << line;
  txt += " : " + msg;

  if(filter != "") {
    QRegExp r(filter);
    if(r.indexIn(txt) == -1) return;
  }

  timer.stop();

  QListWidgetItem *item = new QListWidgetItem();
  item->setText(txt);
  if(cl == _error) item->setBackground(QBrush(QColor(230, 200, 200)));
  if(cl == _warn) item->setBackground(QBrush(QColor(200, 200, 230)));
  if(cl == _log) item->setBackground(QBrush(QColor(200, 230, 200)));

  lst->addItem(item);
  timer.start();
}

static DebugWindow* debugwindow = nullptr;

void dbg_init()
{
  if(debugwindow) dbg_free();
  debugwindow = new DebugWindow();
}

void dbg_free()
{
  if(debugwindow) delete debugwindow;
  debugwindow = nullptr;
}

void dbg_show()
{
  if(debugwindow) debugwindow->show();
}

void dbg_toggle()
{
  if(debugwindow) {
    if(debugwindow->isVisible()) dbg_hide();
    else dbg_show();
  }
}

void dbg_hide()
{
  if(debugwindow) debugwindow->hide();
}

bool dbg_enabled()
{
  return debugwindow != nullptr;
}

void dbg_log(const char *func, const char *file, const int line,
             debug_class cl, const char *ch, const char *fmt, ...)
{
  if(debugwindow) {
    QString msg;

    va_list va;
    va_start(va, fmt);
    msg.vasprintf(fmt, va);
    va_end(va);

    debugwindow->log(func, file, line, cl, ch, msg);
  } else {
    fprintf(stderr, "[%s] %s %s:%s:%d ",
            class_str[(unsigned)cl], ch, file, func, line);
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
  }
}


#ifdef TEST_DEBUG
//Additional dependency files
//deps:
//Required cflags (autoconf vars may be used)
//cflags:
//Required link options (autoconf vars may be used)
//libs:
#include "test.h"

TEST_BEGIN;

// TODO: Put some testcode here (see test.h for usable macros).

TEST_END;

#endif/*TEST_DEBUG*/
