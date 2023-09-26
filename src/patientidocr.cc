/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            patientidocr.cc
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

#include "patientidocr.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QImage>
#include <QPainter>

PatientIdOcr::PatientIdOcr(const int anchorXDelta,
                           const int anchorYDelta,
                           const int maxColorDifference,
                           const bool debug) :
  anchorXDelta(anchorXDelta),
  anchorYDelta(anchorYDelta),
  maxColorDifference(maxColorDifference),
  debug(debug)
{
  numbers["0"] = QImage("data/ocr/0.png").convertToFormat(QImage::Format_ARGB32);
  numbers["1"] = QImage("data/ocr/1.png").convertToFormat(QImage::Format_ARGB32);
  numbers["2"] = QImage("data/ocr/2.png").convertToFormat(QImage::Format_ARGB32);
  numbers["3"] = QImage("data/ocr/3.png").convertToFormat(QImage::Format_ARGB32);
  numbers["4"] = QImage("data/ocr/4.png").convertToFormat(QImage::Format_ARGB32);
  numbers["5"] = QImage("data/ocr/5.png").convertToFormat(QImage::Format_ARGB32);
  numbers["6"] = QImage("data/ocr/6.png").convertToFormat(QImage::Format_ARGB32);
  numbers["7"] = QImage("data/ocr/7.png").convertToFormat(QImage::Format_ARGB32);
  numbers["8"] = QImage("data/ocr/8.png").convertToFormat(QImage::Format_ARGB32);
  numbers["9"] = QImage("data/ocr/9.png").convertToFormat(QImage::Format_ARGB32);
  numbers["-"] = QImage("data/ocr/-.png").convertToFormat(QImage::Format_ARGB32);
}

QString PatientIdOcr::getPatientId()
{
  QList<QImage> anchors;
  QImage anchor = QImage("data/ocr/anchor.png").convertToFormat(QImage::Format_ARGB32);
  QRgb anchorColor(anchor.pixelColor(anchor.width() / 2, 0).rgb());

  bool found = false;
  int idx = 0;
  for(const auto &screen: QGuiApplication::screens()) {
    QPixmap desktop = screen->grabWindow(0);
    if(debug) {
      desktop.save("ocr_desktop" + QString::number(idx) + ".png");
    }
    idx++;
    QImage desktopGrab = desktop.toImage().convertToFormat(QImage::Format_ARGB32);
    QImage progressImage = desktopGrab;

    int x = 0;
    int y = 0;
    // First look for OCR anchor (a place we know must exist in the image)
    for(; y < desktopGrab.height(); ++y) {
      const QRgb *line = (const QRgb *)desktopGrab.constScanLine(y);
      int step = anchor.width();
      for(x = step; x < desktopGrab.width(); x += step) {
        progressImage.setPixelColor(x, y, QColor(Qt::red));
        if(colorCompare(line[x], anchorColor)) {
          for(int a = 0; a <= step; ++a) {
            progressImage.setPixelColor(x - a, y, QColor(Qt::green));
            if(colorCompare(line[x - a], anchorColor)) {
              if(a != step) { // Only continue loop when we are not at the leftmost pixel. Otherwise always proceed to test anchor without '--a' to make sure we test at least once for every anchor width in cases where EPJ is fullscreen and the anchor begins at X = 0
                continue;
              }
            } else {
              --a; // Move forwards 1 pixel (yes, it is --) to make sure anchor matches
            }
            if(imageCompare((const QRgb *)desktopGrab.copy(x - a, y, anchor.width(), anchor.height()).constBits(), (const QRgb *)anchor.constBits(), anchor.width() * anchor.height())) {
              x -= a;
              found = true;
            }
            break;
          }
          if(found) {
            break;
          }
        }
      }
      if(found) {
        break;
      }
    }

    if(debug) {
      progressImage.save("ocr_debug.png");
    }
    // Now do OCR on the numbers at the anchor
    if(found) {
      QString patientId = "";
      // Also move in n pixels. This is the width delta to where the numbers start at the anchor
      x += anchorXDelta;
      // Go down n pixels. This is the height delta to where the numbers start at the anchor
      y += anchorYDelta;
      int b = 0;
      for(int a = 0; a < 256; ++a) {
        progressImage.setPixelColor(x + a, y, QColor(Qt::blue));
        // Now try for each number
        for(const auto &key: numbers.keys()) {
          if(imageCompare((const QRgb *)desktopGrab.copy(x + a, y, numbers[key].width(), numbers[key].height()).constBits(), (const QRgb *)numbers[key].constBits(), numbers[key].width() * numbers[key].height())) {
            //printf("MATCH! '%s' found at %d\n", key.toStdString().c_str(), a);
            patientId.append(key);
            b = a;
            a += numbers[key].width();
            break;
          } else {
            //printf("NO MATCH! '%s' not at %d\n", key.toStdString().c_str(), a);
          }
        }
        if(a - b > numbers["0"].width() * 1.5) {
          //printf("Too far between letters, ending patientId OCR.\n");
          break;
        }
      }
      if(debug) {
        progressImage.save("ocr_debug.png");
      }
      printf("OCR PATIENTID: '%s'\n", patientId.toStdString().c_str());
      return patientId;
    }
  }
  return QString();
}

bool PatientIdOcr::imageCompare(const QRgb *src, const QRgb *dst, const int size)
{
  bool isEqual = true;
  for(int a = 0; a < size; ++a) {
    //printf("Src: r: %d, g: %d, b: %d\n", qRed(src[a]), qGreen(src[a]), qBlue(src[a]));
    //printf("Dst: r: %d, g: %d, b: %d\n\n", qRed(dst[a]), qGreen(dst[a]), qBlue(dst[a]));
    if(abs(qRed(src[a]) - qRed(dst[a])) > maxColorDifference ||
       abs(qBlue(src[a]) - qBlue(dst[a])) > maxColorDifference ||
       abs(qGreen(src[a]) - qGreen(dst[a])) > maxColorDifference) {
      isEqual = false;
      break;
    }
  }
  return isEqual;
}

bool PatientIdOcr::colorCompare(QRgb src, QRgb dst)
{
  if(abs(qRed(src) - qRed(dst)) > 10 ||
     abs(qGreen(src) - qGreen(dst)) > 10 ||
     abs(qBlue(src) - qBlue(dst)) > 10) {
    return false;
  }
  return true;
}
