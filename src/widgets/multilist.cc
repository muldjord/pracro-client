/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            multilist.cc
 *
 *  Mon Jun 16 15:31:24 CEST 2008
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
#include "multilist.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QFrame>
#include <QLabel>
#include <QEvent>
#include <QKeyEvent>

#include "messagebox.h"
#include "common.h"
#include "macrowindow.h"
#include "debug.h"

MultiList::MultiList(QDomNode &node, MacroWindow *macrowindow)
  : Widget(node, macrowindow)
{
  frame = new QFrame();
  setWidget(frame);

  hideChildren = true;

  innerwidget_has_changes = false;
  setCommonAttributes(frame, node);

  QGridLayout *layout = new QGridLayout(frame);
  frame->setLayout(layout);

  list = new QListWidget(frame);
  layout->addWidget(list, 0, 0, 1, 2, Qt::AlignTop);
  list->installEventFilter(this);

  QPushButton *add = new QPushButton(frame);
  connect(add, &QPushButton::clicked, this, &MultiList::add);
  add->setText(tr("Add to list"));
  add->setIcon(QIcon(QPixmap(":add.png")));
  layout->addWidget(add, 1, 0, 1, 1, Qt::AlignTop);

  QPushButton *rem = new QPushButton(frame);
  connect(rem, &QPushButton::clicked, this, &MultiList::remove);
  rem->setText(tr("Remove from list"));
  rem->setIcon(QIcon(QPixmap(":del.png")));
  layout->addWidget(rem, 1, 1, 1, 1, Qt::AlignTop);

  QLabel *arrows = new QLabel(frame);
  arrows->setPixmap(QPixmap(":arrows.png"));
  layout->addWidget(arrows, 2, 0, 1, 2, Qt::AlignHCenter);
  
  QWidget *inputbox = new QWidget(frame);
  inputbox->setContentsMargins(0,0,0,0);
  layout->addWidget(inputbox, 3, 0, 1, 2, Qt::AlignTop);

  QDomElement elem = node.toElement();
  if(elem.hasAttribute("layout")) {
    if(elem.attribute("layout") == "hbox") {
      QHBoxLayout *layout = new QHBoxLayout(inputbox);
      inputbox->setLayout(layout);
    } else if (elem.attribute("layout") == "vbox") {
      QVBoxLayout *layout = new QVBoxLayout(inputbox);
      inputbox->setLayout(layout);      
    } else { // layout not set
      QVBoxLayout *layout = new QVBoxLayout(inputbox);
      inputbox->setLayout(layout);      
    }
  } else {
    QHBoxLayout *layout = new QHBoxLayout(inputbox);
    inputbox->setLayout(layout);
  }

  inputbox->layout()->setContentsMargins(0,0,0,0);
  
  addChildren(node, inputbox->layout());

  innerwidget = nullptr;
  if(elem.hasAttribute("innerwidget")) {
    innerwidget = findWidget(elem.attribute("innerwidget"), true);
    if(innerwidget) {
      connect(innerwidget, &Widget::wasChanged, this, &MultiList::changed);
    } else {
      DEBUG(multilist, "ERROR: Inner Widget %s not found (in multilist)!\n",
            elem.attribute("innerwidget").toStdString().c_str());
    }
  } else {
    DEBUG(multilist, "ERROR: Missing 'innerwidget' attribute on multilist!\n");
  }

  layout->setContentsMargins(0,0,0,0);
}

MultiList::~MultiList()
{
  // delete frame;
}

void MultiList::changed()
{
  innerwidget_has_changes = true;
  DEBUG(multilist, "Multilist innerwidget was changed\n");
}

bool MultiList::preCommit()
{
  if(innerwidget_has_changes) {
    switch(MessageBox::warning(nullptr,
             tr("Inner widget changed."),
             tr("The inner widget changed, and you didn't add it to the list.\n"
                "Do you want to continue and discard the change?"),
             MessageBox::Ignore | MessageBox::Cancel)) {
    case MessageBox::Ignore:
      break;
    case MessageBox::Cancel:
    default:
      return false;
    }
  }

  return true;
}

QString MultiList::value()
{
  QString values;

  QList<QListWidgetItem *> items = list->findItems("*", Qt::MatchWildcard);
  QList<QListWidgetItem *>::iterator i = items.begin();
  while(i != items.end()) {
    QListWidgetItem *item = *i;
    if(values != "") values += "\n";
    values += item->text();
    i++;
  }

  return values;
}

void MultiList::setValue(QString values, QString source)
{
  DEBUG(multilist, "setValue(%s)\n", values.toStdString().c_str());

  if(isUserSource(source)) emit wasChanged();

  QString value;

  list->clear();

  int idx = 0;
  do {
    value = values.section('\n', idx, idx);
    if(value != "") list->addItem(value);
    idx++;
  } while(value != "");

  emit eventOnChange();
}

void MultiList::remove()
{
  QListWidgetItem *item = list->currentItem();

  if(item && item->isSelected()) {
    delete item;
    emit eventOnChange();
    emit wasChanged();
  }
}

void MultiList::add()
{
  if(innerwidget && innerwidget->valid(true)) {
    list->addItem(innerwidget->value());
    emit wasChanged();

    innerwidget->setValues(); // Hack to reset widgets to their initial values.
    innerwidget_has_changes = false;

    emit eventOnChange();
  }
}

void MultiList::setValues()
{
  Widget::setValues();
  innerwidget_has_changes = false;
}

bool MultiList::setKeyboardFocus()
{
  if(innerwidget) return innerwidget->setKeyboardFocus();
  return false;
}

bool MultiList::eventFilter(QObject *obj, QEvent *event)
{
  if(event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if(keyEvent->key() == Qt::Key_Delete) remove();
  }

  return Widget::eventFilter(obj, event);
}

void MultiList::setWdgValid(bool valid)
{
  QPalette palette;

  if(valid) {
    // valid string
    palette.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255)));
  } else {
    // invalid string
    palette.setBrush(QPalette::Base, QBrush(QColor(230, 200, 200)));
  }

  list->setPalette(palette);
}
