/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            textedit.cc
 *
 *  Fri Jul 13 12:38:45 CEST 2007
 *  Copyright 2007 Bent Bisballe Nyeng and Lars Bisballe Jensen
 *  deva@aasimon.org and elsenator@gmail.com
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
#include "textedit.h"

#include <QPalette>
#include <QPlainTextEdit>
#include <stdio.h>

#include "common.h"

TextEdit::TextEdit(QDomNode &node, MacroWindow *macrowindow)
  : Widget(node, macrowindow)
{
  textedit = new QPlainTextEdit();
  setWidget(textedit);

  setCommonAttributes(textedit, node);

  QDomElement elem = node.toElement();

  if(elem.hasAttribute("readonly")) {
    if(elem.attribute("readonly") == "true" ||
       elem.attribute("readonly") == "1") {
      textedit->setReadOnly(true);
    } else if(elem.attribute("readonly") == "false" ||
              elem.attribute("readonly") == "0") {
      textedit->setReadOnly(false);
    } else {
      printf("Unknown value of readonly: %s\n",
             elem.attribute("readonly").toStdString().c_str());
    }
  }

  connect(textedit, &QPlainTextEdit::textChanged, this, &TextEdit::changed);
  textedit->installEventFilter(this); // Detect keyboard input.
}

TextEdit::~TextEdit()
{
  //delete textedit;
}

void TextEdit::changed()
{
  emit eventOnChange();
}

QString TextEdit::value()
{
  return textedit->toPlainText();
}

void TextEdit::setValue(QString value, QString source)
{
  if(isUserSource(source)) emit wasChanged();
  textedit->setPlainText(value);
  //  setInitialValue(value);
}

bool TextEdit::readOnly()
{
  return textedit->isReadOnly();
}

void TextEdit::setReadOnly(bool readOnly)
{
  textedit->setReadOnly(readOnly);
}

bool TextEdit::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if(keyEvent->text() == "\t") {
      event->ignore();
      return true;
    }
    if(keyEvent->text() != "") emit wasChanged();
  }

  return Widget::eventFilter(obj, event);
}

void TextEdit::setWdgValid(bool valid)
{
  QPalette palette;

  if(valid) {
    // valid string
    palette.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255)));
  } else {
    // invalid string
    palette.setBrush(QPalette::Base, QBrush(QColor(230, 200, 200)));
  }

  textedit->setPalette(palette);
}
