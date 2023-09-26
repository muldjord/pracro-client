/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            debug.h
 *
 *  Tue Aug 17 10:48:44 CEST 2010
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
#ifndef __PRACRO_DEBUG_H__
#define __PRACRO_DEBUG_H__

#include <QMainWindow>
#include <QListWidget>
#include <QCloseEvent>
#include <QTimer>
#include <QString>

typedef enum {
  _debug,
  _error,
  _warn,
  _log
} debug_class;

class DebugWindow : public QMainWindow {
  Q_OBJECT
public:
  DebugWindow();

  void log(const char *func, const char *file, const int line,
           debug_class cl, const char *ch, QString &msg);

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void clear();
  void debug_toggled(bool);
  void error_toggled(bool);
  void warning_toggled(bool);
  void log_toggled(bool);
  void filter_changed(QString);

private:
  QListWidget *lst;
  QTimer timer;

  QString filter;

  bool show_debug;
  bool show_error;
  bool show_warning;
  bool show_log;
};

void dbg_init();
void dbg_free();

void dbg_show();
void dbg_toggle();
void dbg_hide();

bool dbg_enabled();


void dbg_log(const char *func, const char *file, const int line,
             debug_class cl, const char *ch, const char *fmt, ...)
  __attribute__((format (printf,6,7)));

#ifdef USE_DEBUG
#define DEBUG(ch, fmt...) \
  dbg_log(__func__, __FILE__, __LINE__, _debug, #ch, fmt);
#else
#define DEBUG(ch, fmt...)
#endif
#define ERR(ch, fmt...)                                 \
  dbg_log(__func__, __FILE__,__LINE__, _error, #ch, fmt);
#define WARN(ch, fmt...) \
  dbg_log(__func__, __FILE__,__LINE__, _warn, #ch, fmt);
#define LOG(ch, fmt...) \
  dbg_log(__func__, __FILE__,__LINE__, _log, #ch, fmt);

#endif/*__PRACRO_DEBUG_H__*/
