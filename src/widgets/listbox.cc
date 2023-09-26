/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            listbox.cc
 *
 *  Wed Jul 18 10:35:52 CEST 2007
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
#include "listbox.h"

#include <QListWidgetItem>
#include <QListWidget>

#include "common.h"
#include "luawidget.h"

static QListWidgetItem *createItem(QDomElement &elem)
{
  QListWidgetItem *item = new QListWidgetItem();

  if(elem.hasAttribute("caption") &&
     elem.hasAttribute("value")) {
    item->setText(elem.attribute("caption"));
    item->setData(Qt::UserRole, elem.attribute("value"));
  } else {
    printf("XML Error!!! Listbox item is missing one or more "
           "attributes...\n");
  }

  if(elem.hasAttribute("type")) {

    if(elem.attribute("type") == "header") {
      item->setFlags(Qt::NoItemFlags);

      QFont headerfont;
      headerfont.setBold(true);
      headerfont.setItalic(true);
      item->setFont(headerfont);

      QBrush headerbrush(Qt::SolidPattern);
      headerbrush.setColor(Qt::lightGray);
      item->setBackground(headerbrush);

    }

    if(elem.attribute("type") == "separator") {
      item->setText("");
      item->setFlags(Qt::NoItemFlags);
    } 

  }

  return item;
}

ListBox::ListBox(QDomNode &node, MacroWindow *macrowindow)
  : Widget(node, macrowindow)
{
  listwidget = new QListWidget();
  setWidget(listwidget);

  valueIsChangingByComputer = false;

  setCommonAttributes(listwidget, node);

  QDomNodeList children = node.childNodes();

  for (int i=0; i < children.count(); i++) {
    QDomNode child = children.at(i);
    QDomElement list_elem = child.toElement();
    listwidget->addItem(createItem(list_elem));
  }

  connect(listwidget, &QListWidget::itemSelectionChanged, this, &ListBox::changed);
}

ListBox::~ListBox()
{
  // delete listwidget;
}

bool ListBox::preValid()
{
  return listwidget->selectedItems().size() != 0;
}

QString ListBox::value()
{
  QString value = "none";
  if(listwidget->currentRow() != -1)
    value = listwidget->currentItem()->data(Qt::UserRole).toString();
  return value;
}

void ListBox::setValue(QString value, QString source)
{
  if(isUserSource(source)) emit wasChanged();

  valueIsChangingByComputer = true;
  int sel = -1; // -1 is default for none selected

  for(int i = 0; i < listwidget->count(); i++) {
    QListWidgetItem *listitem = listwidget->item(i);
    if(listitem->data(Qt::UserRole).toString() == value) sel = i;
  }

  listwidget->setCurrentRow(sel);

  //  setInitialValue(value);
  valueIsChangingByComputer = false;
}

void ListBox::changed()
{
  if(!valueIsChangingByComputer) emit wasChanged();
}

void ListBox::setWdgValid(bool valid)
{
  QPalette palette;

  if(valid) {
    // valid string
    palette.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255)));
  } else {
    // invalid string
    palette.setBrush(QPalette::Base, QBrush(QColor(230, 200, 200)));
  }

  listwidget->setPalette(palette);
}

void ListBox::clear()
{
  listwidget->clear();
}

void ListBox::addItem(QString text, QString type)
{
  QDomDocument doc;
  QDomElement elem = doc.createElement("item");
  elem.setAttribute("caption", text);
  elem.setAttribute("value", text);
  if(type != "") elem.setAttribute("type", type);
  listwidget->addItem(createItem(elem));
}

int lst_clear(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "listbox expected");

  ListBox *lst = (ListBox*)wdgu->widget;
  lst->clear();

  return 0;
}

int lst_add_item(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "listbox expected");

  QString val = luaL_checkstring(L, 2);

  ListBox *lst = (ListBox*)wdgu->widget;
  lst->addItem(val, "");

  return 0;
}

int lst_add_header(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "listbox expected");

  QString val = luaL_checkstring(L, 2);

  ListBox *lst = (ListBox*)wdgu->widget;
  lst->addItem(val, "header");

  return 0;
}

int lst_add_separator(lua_State *L)
{
  wdg_userdata *wdgu;

  wdgu = (wdg_userdata *)lua_touserdata(L, 1);
  luaL_argcheck(L, wdgu, 1, "listbox expected");

  ListBox *lst = (ListBox*)wdgu->widget;
  lst->addItem("", "separator");

  return 0;
}
