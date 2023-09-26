/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            patientidocr.h
 *
 *  Mon May 11 13:50:00 CEST 2020
 *  Copyright 2020 Lars Bisballe Jensen
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
#ifndef __PRACRO_PATIENTIDOCR_H__
#define __PRACRO_PATIENTIDOCR_H__

#include <QObject>
#include <QColor>
#include <QMap>
#include <QImage>

class PatientIdOcr : public QObject
{
  Q_OBJECT

public:
  PatientIdOcr(const int xDelta,
               const int yDelta,
               const int maxColorDifference,
               const bool debug);
  QString getPatientId();

private:
  int anchorXDelta = 0;
  int anchorYDelta = 0;
  int maxColorDifference = 0;
  bool debug = false;
  QMap<QString, QImage> numbers;
  bool imageCompare(const QRgb *src, const QRgb *dst, const int size);
  bool colorCompare(QRgb src, QRgb dst);

};

#endif/*__PRACRO_PATIENTIDOCR_H__*/
