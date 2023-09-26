/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            macrodrawer.h
 *
 *  Tue Jan  6 15:49:48 CET 2009
 *  Copyright 2009 Bent Bisballe Nyeng
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
#ifndef __PRACRO_MACRODRAWER_H__
#define __PRACRO_MACRODRAWER_H__

#include "expandbutton.h"

#include <QGroupBox>
#include <QString>
#include <QPushButton>

class Macro;
class MacroDrawer : public QGroupBox {
Q_OBJECT
public:
  MacroDrawer(Macro *macro, QString title);
  ~MacroDrawer();

protected:
  bool eventFilter(QObject *obj, QEvent *event);

public slots:
  void toggleMe();
  void activationChange(bool active);
  void setBusyTitle();
  void revertTitle();

signals:
  void toggle();

private:
  QWidget *edge;
  ExpandButton *button;
  QString busyTitle;
  QString macroTitle;
};

#endif/*__PRACRO_MACRODRAWER_H__*/
