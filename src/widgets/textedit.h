/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            textedit.h
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
#ifndef __PRACRO_TEXTEDIT_H__
#define __PRACRO_TEXTEDIT_H__

#include "widget.h"

#include <QDomNode>
#include <QKeyEvent>

/***
 * Multiline Text Edit Widget
 * @tag textedit
 * @clientside
 * @screenshot Example of a textedit.
 * <textedit name="x" value="This is a multiline textfield.
 * It may contain all text in the universe, it will simply add a scrollbar."/>
 * @extends widget
 * The textedit is a multiline textual input field. Unlike the @see lineedit it
 * accepts the newline character which will also be part of its resulting value.
 * @att readonly Make the textedit readonly (not changeable by the user), but
 * still able to select and copy text.
 */

class QPlainTextEdit;
class TextEdit : public Widget
{
Q_OBJECT
public:
  TextEdit(QDomNode &node, MacroWindow *macrowindow);
  ~TextEdit();

  QString value();
  void setValue(QString value, QString source);

  bool readOnly() override;
  void setReadOnly(bool readOnly) override;

  void setWdgValid(bool valid);

public slots:
  void changed();
  //  void user_changed();

protected:
  bool eventFilter(QObject *obj, QEvent *event);

private:
  QPlainTextEdit *textedit;
};

#endif/*__PRACRO_TEXTEDIT_H__*/
