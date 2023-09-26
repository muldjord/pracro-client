/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            resumewidget.cc
 *
 *  Tue Sep 23 12:02:03 CEST 2008
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
#include "resumewidget.h"

#include <QHBoxLayout>
#include <QBoxLayout>
#include <QTextEdit>
#include <QDialog>
#include <QFontDatabase>

#define MAX_COMPACT_SIZE 100

//#define RICH // Experimental syntax highlighter (numbers turn blue)
#define FIXED_FONT // Show the resume, using a fixed font.

ResumeWidget::ResumeWidget(bool compact)
{
  setAutoFillBackground(true);

  this->compact = compact;
  setLayout(new QHBoxLayout());
  layout()->setContentsMargins(10,2,2,2);
  resume = new QLabel("<font style='color: #bbb;'>Endnu ikke udfyldt</font>");

#ifdef RICH
  resume->setTextFormat(Qt::RichText);
#endif

  resume->setWordWrap(true);

  layout()->addWidget(resume);

  connect(resume, &QLabel::linkActivated, this, &ResumeWidget::showFull);
}

#define LINEWIDTH 80
static QString reformatString(QString help)
{
  QString output;

  int lastnl = 0;
  for(int i = 0; i < help.size(); i++) {
    if(help[i] == '\n') {
      lastnl = 0;
    } else {
      if(lastnl > LINEWIDTH && help[i] == ' ') {
        output += '\n';
        lastnl = 0;
      } else {
        lastnl++;
      }
    }

    output += help[i];

  }
  return output;
}

void ResumeWidget::setText(QString text, state_t state)
{
  QString f;

  fulltext = text;

#ifdef FIXED_FONT
  QFont fixedfont("Bitstream Vera Sans Mono", 8, QFont::Normal);
  resume->setFont(fixedfont);
#endif

#ifdef RICH
  for(int i = 0; i < text.length(); i++) {
    if(text[i] >= '0' && text[i] <= '9') f += "<font style='color: #55F;'>" + text[i] + "</font>";
    else if(text[i] == '\n') f += "<br/>";
    else if(text[i] == ' ') f += "&nbsp;";
    else f += text[i];
  }

  resume->setWordWrap(true);
#else
  f = text;
#endif

  if(compact) {
    if(f.count('\n') > 0) f = f.left(f.indexOf('\n')); // Limit to one line.
    
    if(f.length() > MAX_COMPACT_SIZE) {
      f = f.left(MAX_COMPACT_SIZE); // limit to first MAX_COMPACT_SIZE characters.
    }
    
    if(fulltext != f) {
      f += " <a href=\"#\"><img src=\":compressed.png\"/></a>";
      //resume->setToolTip(reformatString(fulltext)); // Only set tooltip if resume has actually been cut off.
      //resume->setWhatsThis(fulltext); // Only set tooltip if resume has actually been cut off.
    }
  }

  switch(state) {
  case NEW:
    {
      QPalette palette;
      palette.setBrush(foregroundRole(), QBrush(QColor(0, 0, 255)));
      resume->setPalette(palette); 
    }
    break;
  case OLD:                                     
    break;
  case DIRTY:
    {
      QPalette palette;
      palette.setBrush(foregroundRole(), QBrush(QColor(255, 0, 0)));
      resume->setPalette(palette); 
    }
    break;
  }

  int num = f.length() / 90;
  for(int i = 0; i < num; i++) f+="\n";

  resume->setText(f);
}

QString ResumeWidget::getText()
{
  return fulltext;
}

void ResumeWidget::showFull(const QString &)
{
  QDialog d(this);
  d.setWindowTitle("Resume");

  QTextEdit *t = new QTextEdit(&d);
  d.resize(500, 200);
  d.move(mapToGlobal(pos()));
  t->setText(reformatString(fulltext));
  t->setReadOnly(true);
  t->setLineWrapMode(QTextEdit::NoWrap);

  d.setLayout(new QHBoxLayout());
  d.layout()->addWidget(t);

  d.exec();
}
