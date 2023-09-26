/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            messagebox.cc
 *
 *  Wed Mar 18 13:33:05 CET 2009
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
#include "messagebox.h"

#include <QDialogButtonBox>
#include <QPushButton>
/*
  QString p = tr("KEY", "default vaerdi");
  printf("\n\n\n#######################\n%s\n\n\n", p.toStdString().c_str());
*/

static MessageBox::StandardButton showNewMessageBox(QWidget *parent,
                                                    MessageBox::Icon icon,
                                                    const QString& title, const QString& text,
                                                    MessageBox::StandardButtons buttons,
                                                    MessageBox::StandardButton defaultButton)
{
  QMessageBox msgBox(icon, title, text, MessageBox::NoButton, parent);
  QDialogButtonBox *buttonBox = msgBox.findChild<QDialogButtonBox*>();

  Q_ASSERT(buttonBox != 0);

  uint mask = MessageBox::FirstButton;
  while(mask <= MessageBox::LastButton) {
    uint sb = buttons & mask;
    mask <<= 1;

    if(!sb) continue;

    QPushButton *button = msgBox.addButton((MessageBox::StandardButton)sb);

    switch(sb) {
    case MessageBox::Ok: // An "OK" button defined with the AcceptRole.
      button->setText(QObject::tr("Ok"));
      break;
    case MessageBox::Open: // A "Open" button defined with the AcceptRole.
      button->setText(QObject::tr("Open"));
      break;
    case MessageBox::Save: // A "Save" button defined with the AcceptRole.
      button->setText(QObject::tr("Save"));
      break;
    case MessageBox::Cancel: // A "Cancel" button defined with the RejectRole.
      button->setText(QObject::tr("Cancel"));
      break;
    case MessageBox::Close: // A "Close" button defined with the RejectRole.
      button->setText(QObject::tr("Close"));
      break;
    case MessageBox::Discard: // A "Discard" or "Don't Save" button, depending on the platform, defined with the DestructiveRole.
      button->setText(QObject::tr("Discard"));
      break;
    case MessageBox::Apply: // An "Apply" button defined with the ApplyRole.
      button->setText(QObject::tr("Apply"));
      break;
    case MessageBox::Reset: // A "Reset" button defined with the ResetRole.
      button->setText(QObject::tr("Reset"));
      break;
    case MessageBox::RestoreDefaults: // A "Restore Defaults" button defined with the ResetRole.
      button->setText(QObject::tr("Restore Defaults"));
      break;
    case MessageBox::Help: // A "Help" button defined with the HelpRole.
      button->setText(QObject::tr("Help"));
      break;
    case MessageBox::SaveAll: // A "Save All" button defined with the AcceptRole.
      button->setText(QObject::tr("Save All"));
      break;
    case MessageBox::Yes: // A "Yes" button defined with the YesRole.
      button->setText(QObject::tr("Yes"));
      break;
    case MessageBox::YesToAll: // A "Yes to All" button defined with the YesRole.
      button->setText(QObject::tr("Yes To All"));
      break;
    case MessageBox::No: // A "No" button defined with the NoRole.
      button->setText(QObject::tr("No"));
      break;
    case MessageBox::NoToAll: // A "No to All" button defined with the NoRole.
      button->setText(QObject::tr("No To All"));
      break;
    case MessageBox::Abort: // An "Abort" button defined with the RejectRole.
      button->setText(QObject::tr("Abort"));
      break;
    case MessageBox::Retry: // A "Retry" button defined with the AcceptRole.
      button->setText(QObject::tr("Retry"));
      break;
    case MessageBox::Ignore: // An "Ignore" button defined with the AcceptRole.
      button->setText(QObject::tr("Ignore"));
      break;
    case MessageBox::NoButton: // An invalid button.
      break;
    }

    // Choose the first accept role as the default
    if(msgBox.defaultButton()) continue;
    if((defaultButton == MessageBox::NoButton 
        && buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
       || (defaultButton != MessageBox::NoButton && sb == uint(defaultButton)))
      msgBox.setDefaultButton(button);
  }

  if (msgBox.exec() == -1) return MessageBox::Cancel;

  return msgBox.standardButton(msgBox.clickedButton());
}

MessageBox::StandardButton MessageBox::information(QWidget *parent, const QString &title,
                                                   const QString& text, StandardButtons buttons,
                                                   StandardButton defaultButton)
{
  return showNewMessageBox(parent, Information, title, text, buttons, defaultButton);
}

MessageBox::StandardButton MessageBox::question(QWidget *parent, const QString &title,
                                                const QString& text, StandardButtons buttons,
                                                StandardButton defaultButton)
{
  return showNewMessageBox(parent, Question, title, text, buttons, defaultButton);
}
MessageBox::StandardButton MessageBox::warning(QWidget *parent, const QString &title,
                                               const QString& text, StandardButtons buttons,
                                               StandardButton defaultButton)
{
  return showNewMessageBox(parent, Warning, title, text, buttons, defaultButton);
}

MessageBox::StandardButton MessageBox::critical(QWidget *parent, const QString &title,
                                                const QString& text, StandardButtons buttons,
                                                StandardButton defaultButton)
{
  return showNewMessageBox(parent, Critical, title, text, buttons, defaultButton);
}
