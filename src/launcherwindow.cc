/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            launcherwindow.cc
 *
 *  Tue Oct 12 11:38:47 CEST 2010
 *  Copyright 2010 Lars Bisballe Jensen
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

#include "launcherwindow.h"
#include "strtools.h"

#include <QApplication>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QRegularExpression>
#include <QMessageBox>
#include <QIcon>
#include <QFile>
#include <QLabel>

LauncherWindow::LauncherWindow(QString patientId, const QList<QString> &runEntries, QWidget *parent)
  : QDialog(parent), patientIdUndo(patientId)
{
  setWindowTitle(tr("Pracro Launcher") + " v" VERSION);
  //resize(350,150);

  setWindowIcon(QIcon(":icon.png"));
  
  QLabel *iconHeader = new QLabel;
  iconHeader->setPixmap(QPixmap(":icon.png"));

  patientIdLineEdit = new QLineEdit();
  patientIdLineEdit->setPlaceholderText("DDMMYY-XXXX");
  patientIdLineEdit->setText(patientId);
  QRegExpValidator *patientIdValidator = new QRegExpValidator(QRegExp("[0-9]{6}-[a-zA-Z0-9]{4}"));
  patientIdLineEdit->setValidator(patientIdValidator);
  connect(patientIdLineEdit, &QLineEdit::textEdited, this, &LauncherWindow::patientIdChanged);

  runComboBox = new QComboBox();
  for(const auto &runEntry: runEntries) {
    QList<QString> snippets = runEntry.split(';');
    if(snippets.length() == 3 || snippets.length() == 2) {
      QString value = snippets.at(1);
      QString title = value;
      if(snippets.length() == 3) {
        title = snippets.at(2);
      }
      runComboBox->addItem(title, value);
      runComboBox->setItemData(runComboBox->count() - 1, runEntry, Qt::UserRole);
    }
  }
  // Set default course if it exists
  /*
  if(runComboBox->findData(course) != -1) {
    runComboBox->setCurrentIndex(runComboBox->findData(course));
  } else if(runComboBox->findText(course) != -1) {
    runComboBox->setCurrentIndex(runComboBox->findText(course));
  }
  */
  
  QFormLayout *formLayout = new QFormLayout();
  formLayout->addRow(tr("Patient ID:"), patientIdLineEdit);
  formLayout->addRow(tr("Start:"), runComboBox);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                     QDialogButtonBox::Cancel);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &LauncherWindow::checkFields);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(iconHeader, 0, Qt::AlignCenter);
  layout->addLayout(formLayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
  patientIdLineEdit->setFocus();
}

LauncherWindow::~LauncherWindow()
{
}

QString LauncherWindow::getPatientId()
{
  return patientIdLineEdit->text();
}

QString LauncherWindow::getRunEntry()
{
  return QString(runComboBox->currentData(Qt::UserRole).toString());
}

void LauncherWindow::patientIdChanged(const QString &text)
{
  if(patientIdUndo.length() == 8 && text.length() == 7) {
    patientIdLineEdit->setText(patientIdLineEdit->text().left(6));
  } else if(text.length() == 6) {
    patientIdLineEdit->setText(patientIdLineEdit->text() + "-");
  } else if(text.length() == 8 && text.mid(6, 2) == "--") {
    patientIdLineEdit->setText(patientIdLineEdit->text().left(7));
  } else if(text.length() > 11) {
    patientIdLineEdit->setText(patientIdLineEdit->text().left(11));
  } else if(patientIdUndo.length() == 8 && text.length() == 7) {
    patientIdLineEdit->setText(patientIdLineEdit->text().left(6));
  }
  patientIdUndo = text;
}

void LauncherWindow::checkFields()
{
  if(!StrTools::isPatientId(patientIdLineEdit->text())) {
    QMessageBox::critical(this, tr("Incorrect patient id"),
                          tr("The format must be 'DDMMYY-XXXX'."),
                          QMessageBox::Ok);
    patientIdLineEdit->setFocus();
    return;
  }
  if(runComboBox->count() == 0 ||
     runComboBox->itemText(runComboBox->currentIndex()).isEmpty() ||
     runComboBox->itemData(runComboBox->currentIndex()).isNull()) {
    QMessageBox::critical(this, tr("Run selection is invalid"),
                          tr("Run selection is either empty or has an invalid selection"),
                          QMessageBox::Ok);
    runComboBox->setFocus();
    return;
  }
  
  emit accept();
}
