/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lineedit.cc
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
#include "lineedit.h"
#include <stdio.h>

#include <QCoreApplication>
#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>

#include <QCompleter>
#include <QStringListModel>

#include "common.h"
#include "debug.h"
#include "luawidget.h"

LineEdit::LineEdit(QDomNode &node, MacroWindow *macrowindow)
  : Widget(node, macrowindow)
{
  lineedit = new QLineEdit();
  setWidget(lineedit);

  setCommonAttributes(lineedit, node);

  QDomElement elem = node.toElement();

  if(elem.hasAttribute("readonly")) {
    if(elem.attribute("readonly") == "true" ||
       elem.attribute("readonly") == "1") {
      lineedit->setReadOnly(true);
    } else if(elem.attribute("readonly") == "false" ||
              elem.attribute("readonly") == "0") {
      lineedit->setReadOnly(false);
    } else {
      printf("Unknown value of readonly: %s\n",
             elem.attribute("readonly").toStdString().c_str());
    }
  }
  
  connect(lineedit, &QLineEdit::textChanged, this, &LineEdit::changed);
  connect(lineedit, &QLineEdit::textEdited, this, &LineEdit::user_changed);

  lineedit->installEventFilter(this); // Detect keyboard input.

  QCompleter *comp = new QCompleter();
  comp->setCaseSensitivity(Qt::CaseInsensitive);
  QStringListModel *mdl = new QStringListModel();
  comp->setModel(mdl);
  //  comp->popup()->setWindowOpacity(0.3);
  lineedit->setCompleter(comp);
}

LineEdit::~LineEdit()
{
  //  delete lineedit;
}

void LineEdit::changed()
{
  //  if(lineedit->completer()->popup()->isVisible()) return;
  emit eventOnChange();
}

void LineEdit::user_changed()
{
  emit eventOnChange();
  emit wasChanged(); 
}

QString LineEdit::value()
{
  return lineedit->text();
}

void LineEdit::setValue(QString value, QString source)
{
  if(isUserSource(source)) emit wasChanged();

  // Hack to make sure the textChanged signal is emitted.
  if(lineedit->text() == value) lineedit->setText(value + " ");

  lineedit->setText(value);

  //  setInitialValue(value);
}

bool LineEdit::readOnly()
{
  return lineedit->isReadOnly();
}

void LineEdit::setReadOnly(bool readOnly)
{
  lineedit->setReadOnly(readOnly);
}

bool LineEdit::eventFilter(QObject *obj, QEvent *event)
{
  if(event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if(keyEvent->key() == Qt::Key_Return ||
       keyEvent->key() == Qt::Key_Enter) {
      QKeyEvent tabevent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier); 
      QCoreApplication::sendEvent(lineedit, &tabevent);
    }
  }

  return Widget::eventFilter(obj, event);
}

void LineEdit::changeEvent(QEvent *event)
{
  if(event->type() == QEvent::EnabledChange) {
    changed();
  }
}

void LineEdit::setWdgValid(bool valid)
{
  DEBUG(lineedit, "Set valid(%s)\n", valid?"true":"false");

  QPalette palette;

  if(valid) {
    // valid string
    palette.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255)));
  } else {
    // invalid string
    palette.setBrush(QPalette::Base, QBrush(QColor(230, 200, 200)));
  }

  lineedit->setPalette(palette);
}

void LineEdit::clearSuggestions()
{
  QCompleter *comp = lineedit->completer();
  QStringListModel *mdl = (QStringListModel *)comp->model();
  suggestions.clear();
  mdl->setStringList(suggestions);
}

void LineEdit::addSuggestion(QString suggestion)
{
  suggestions << suggestion;
  suggestions.sort();

  QCompleter *comp = lineedit->completer();
  QStringListModel *mdl = (QStringListModel *)comp->model();
  mdl->setStringList(suggestions);
}

bool LineEdit::isSuggested(QString val)
{
  return suggestions.contains(val);
}

void LineEdit::showSuggestions()
{
  QCompleter *comp = lineedit->completer();
  comp->complete();
}


int lin_clear_suggestions(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "lineedit expected");

  LineEdit *led = (LineEdit*)wdgu->widget;
  led->clearSuggestions();

  return 0;
}

int lin_show_suggestions(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "lineedit expected");

  LineEdit *led = (LineEdit*)wdgu->widget;
  led->showSuggestions();

  return 0;
}

int lin_is_suggested(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "lineedit expected");

  const char *val = luaL_checkstring(L, 2);

  LineEdit *led = (LineEdit*)wdgu->widget;

  lua_pushboolean(L, led->isSuggested(val));

  return 1;
}

int lin_add_suggestion(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "lineedit expected");

  const char *val = luaL_checkstring(L, 2);

  LineEdit *led = (LineEdit*)wdgu->widget;
  led->addSuggestion(val);

  return 0;
}
