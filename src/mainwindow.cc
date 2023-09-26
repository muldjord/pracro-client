/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mainwindow.cc
 *
 *  Wed Sep 17 09:41:09 CEST 2008
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
#include "mainwindow.h"

#include <QApplication>
#include <QDomDocument>
#include <QDomNodeList>
#include <QDomNode>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
#include <QEvent>
#include <QCloseEvent>
#include <QDockWidget>
#include <QTimer>
#include <QClipboard>
#include <QFileInfo>

#include "aboutbox.h"
#include "messagebox.h"

#include "macrodrawer.h"
#include "macrowindow.h"

#include "macro.h"
#include "header.h"
#include "template.h"

#include "debug.h"

class Dbg : public QLabel {
public:
  Dbg() {
    setText("dbg");
  }

  void mousePressEvent(QMouseEvent *)
  {
    dbg_toggle();
  }
};

MainWindow *gmainwindow = nullptr;

MainWindow::MainWindow(QString patientid, QString title, QString course,
                       QString host, quint16 port,bool usehttps, bool certverify, QString user)
  : QMainWindow(0), // Qt::WindowContextHelpButtonHint
    netcom(host, port, usehttps, certverify)
{
  gmainwindow = this;

  closing = false;

  configFile = "./configs/" + (user.isNull() || user.isEmpty()?"_generic":user) + ".ini";

  scrollarea = new QScrollArea();
  scrollarea->setObjectName("pracro"); // For use with saveState
  setCentralWidget(scrollarea);
  central = new QWidget();
  scrollarea->setWidget(central);
  scrollarea->setWidgetResizable(true);

  scrollarea->setFocusPolicy(Qt::NoFocus);
  central->setFocusPolicy(Qt::NoFocus);

  central->setLayout(new QVBoxLayout());

  connect(&netcom, &NetCom::gotReply, this, &MainWindow::handle);
  connect(&netcom, &NetCom::networkError, this, &MainWindow::showError);

  isStored = false;

  header = nullptr;

  this->patientid = patientid;
  this->user = user;
  this->course = course;
  this->templ = templ;
  if(title.left(10).toLong()) {
    this->patientTitle = title.left(6) + "-" + title.mid(6);
  } else {
    this->patientTitle = title;
  }

  setWindowTitle("Pracro - " + patientTitle + " (" + user + ")");
  setWindowIcon(QIcon(":/icons/icon.png"));

  QStatusBar *status = statusBar();

  aboutButton = new QPushButton("Pracro v" VERSION, status);
  aboutButton->setIcon(QIcon(":icon.png"));
  aboutButton->setMaximumHeight(18);
  aboutButton->setFocusPolicy(Qt::NoFocus);
  connect(aboutButton, &QPushButton::clicked, this, &MainWindow::showAbout);

  status->addPermanentWidget(aboutButton);

  if(dbg_enabled()) {
    status->addPermanentWidget(new Dbg());
  }

  QToolBar *toolbar = addToolBar("controls");
  toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  toolbar->setObjectName("toolbar"); // For use with saveState

  QAction *close_commit = toolbar->addAction(tr("Close and commit"));
  close_commit->setShortcut(QKeySequence(tr("Ctrl+Alt+S")));
  close_commit->setIcon(QPixmap(":icon_close_commit.png"));

  QAction *close_no_commit = toolbar->addAction(tr("Close no commit"));
  close_no_commit->setIcon(QPixmap(":icon_close_no_commit.png"));
  close_no_commit->setShortcut(QKeySequence(tr("Ctrl+Alt+X")));

  toolbar->addSeparator();

  patientid_label = new QLabel(patientTitle);
  QFont patientid_font = patientid_label->font();
  patientid_font.setBold(true);
  patientid_font.setPointSize(patientid_font.pointSize() + 12);
  patientid_label->setFont(patientid_font);
  toolbar->addWidget(patientid_label);

  connect(close_commit, &QAction::triggered, this, &MainWindow::closeCommit);
  connect(close_no_commit, &QAction::triggered, this, &MainWindow::closeNoCommit);

  setStatusBar(status);

  focusTimer.setInterval(3000);
  focusTimer.setSingleShot(false);
  connect(&focusTimer, &QTimer::timeout, this, &MainWindow::checkFocus);
  focusTimer.start();
  
  QTimer::singleShot(100, this, &MainWindow::init);
}

MainWindow::~MainWindow()
{
  QSettings settings(configFile, QSettings::IniFormat);
  
  if(settings.isWritable()) {
    settings.setValue("main/state", saveState());
    settings.setValue("main/geometry", saveGeometry());
  }
}

void MainWindow::showAbout()
{
  AboutBox aboutBox(this, Qt::Dialog);
  aboutBox.exec();
}

bool MainWindow::hasOpen(void *me)
{
  Entities::iterator i = entities.begin();
  while(i != entities.end()) {
    if(me != i.value() && i.value()->isOpen()) return true;
    i++;
  }
  return false;
}

void MainWindow::copyToClipBoard()
{
  QString allResumees = "";
  Entities::iterator i = entities.begin();
  while(i != entities.end()) {
    Entity *e = *i;
    QString type = e->type();
    if(type == "macro") {
      Macro *m = (Macro*)e;
      if(!m->window->resumewidget->getText().isEmpty()) {
        allResumees.append(m->window->resumewidget->getText().trimmed() + "\n\n");
      }
    }
    i++;
  }

  QApplication::clipboard()->setText(allResumees.trimmed());
}

void MainWindow::enableEntities(bool enable)
{
  Entities::iterator i = entities.begin();
  while(i != entities.end()) {
    i.value()->widget()->setEnabled(enable);
    i++;
  }
}

void MainWindow::closeCommit()
{
  if(closing) return;

  if(hasOpen(nullptr)) {
    MessageBox::warning(this, tr("Close first."),
                        tr("Close open macros first."));
    return;
  }

  if(lostFocus &&
     MessageBox::question(this, tr("Confirm patient context"),
                          tr("It would seem the Pracro window has lost focus during the current procedure. Please ensure that the patient context is intact before proceeding.\n\nPracro contains data for patient-ID:") + " " + patientid + "\n\n" + tr("Is this context correct?"),
                          QMessageBox::No | QMessageBox::Yes) == QMessageBox::No) {
    return;
  }

  // Concatinate and copy all resumes to clipboard
  copyToClipBoard();

  enableEntities(false);

  netcom.commit();
  //  isStored = true;
  closing = true;
  setEnabled(false);
  patientid_label->setText(tr("Committing, please wait..."));
}

void MainWindow::closeNoCommit()
{
  if(closing) return;

  if(hasOpen(nullptr)) {
    MessageBox::warning(this, tr("Close first."),
                        tr("Close open macros first."));
    return;
  }

  enableEntities(false);

  netcom.nocommit();
  //  isStored = true;
  closing = true;
  setEnabled(false);
  patientid_label->setText(tr("Saving, please wait..."));
}

void MainWindow::closeDiscard()
{
  if(closing) return;

  if(hasOpen(nullptr)) {
    MessageBox::warning(this, tr("Close first."),
                        tr("Close open macros first."));
    return;
  }

  MessageBox::StandardButton res =
    MessageBox::critical(this,
                        tr("Discard"),
                        tr("This session will <strong>NOT</strong> be "
                           "stored in the journal.<br/>"
                           "Are you sure you want to continue?"),
                        MessageBox::Yes | MessageBox::No);
  
  if(res == MessageBox::Yes) {
    enableEntities(false);

    netcom.discard();
    //    isStored = true;
    closing = true;
    setEnabled(false);
    patientid_label->setText(tr("Closing, please wait..."));
  }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if(closing) {
    emit isClosing();
    return;
  }

  if(hasOpen(nullptr)) {
    MessageBox::warning(this, tr("Close first."),
                        tr("Close open macros first."));
    event->ignore();
    return;
  }

  if(netcom.sessionid == "" ||
     isStored ||
     MessageBox::critical(this,
                          tr("Discard"),
                          tr("This session will "
                             "<strong>NOT</strong> be stored in "
                             "the journal.<br/>"
                             "Are you sure you want to continue?"),
                          MessageBox::Yes | MessageBox::No) == MessageBox::Yes)
    {
    if(netcom.sessionid != "" && !isStored) {
      netcom.discard();
      //      isStored = true;
      closing = true;
      event->ignore();
      return;
    }

    event->accept();
    dbg_free();
  } else {
    event->ignore();
  }

  emit isClosing();
}

void MainWindow::init()
{
  if(!QFileInfo::exists(configFile)) {
    configFile = "./pracro.ini";
  }
  QSettings settings(configFile, QSettings::IniFormat);

  restoreGeometry(settings.value("main/geometry").toByteArray());
  qApp->processEvents();
  restoreState(settings.value("main/state").toByteArray());

  netcom.patientid = patientid;
  netcom.templ = templ;
  netcom.course = course;
  netcom.user = user;

  netcom.initConnection();
}

void MainWindow::updateTemplateHeaders(QDomNode &node)
{
  QDomElement elem = node.toElement();

  if(elem.tagName() != "template") return;

  QString title = elem.attribute("title");
  QString name = elem.attribute("name");

  if(!header) {
    header = new QLabel();
    QFont headerfont = header->font();
    headerfont.setBold(true);
    headerfont.setPointSize(headerfont.pointSize() + 6);
    header->setFont(headerfont);
    header->setAlignment(Qt::AlignHCenter);
    central->layout()->addWidget(header);
  }

  header->setText(title);

  statusBar()->showMessage(title + " (" + name + ") - SessionID: " + netcom.sessionid);
}

void MainWindow::showError(QString title, QString text)
{
  MessageBox::critical(this, title, text);
}

void MainWindow::updateMacros(QDomNodeList &nodes)
{
  bool creation = entities.size() == 0;

  for(int j = 0; j < nodes.count(); j++) {
    QDomNode node = nodes.at(j);
    QDomElement elem = node.toElement();

    if(elem.tagName() == "macro" || elem.tagName() == "header") {
      QString name = elem.attribute("name");
      
      if(entities.find(j) == entities.end()) {
        Entity *e = nullptr;
        if(elem.tagName() == "macro") {
          e = new Macro(entities, netcom, course, templ, scrollarea);
        }
        if(elem.tagName() == "header") {
          e = new Header();
        }
        entities[j] = e;
        entities[j]->update(node);
        central->layout()->addWidget(e->widget());
      } else {
        entities[j]->update(node);
      }
      continue;
    }

    showError("XML Error", "Expected macro/header tag. Got '" +
              elem.tagName() + "' tag.");
    continue;
  }

  if(creation) {
    for(int i = 0; i < entities.size(); i++) {
      Macro *macro = (Macro*)entities[i];
      if(macro->isstatic == false) {
        macro->widget()->setFocus();
        break;
      }
    }

    QVBoxLayout *l = (QVBoxLayout*)central->layout();
    l->addStretch(1);
  }
}

void MainWindow::updateTemplates(QDomNodeList &nodes)
{
  for(int j = 0; j < nodes.count(); j++) {
    QDomNode node = nodes.at(j);
    QDomElement elem = node.toElement();

    if(elem.tagName() == "template") {
      QString name = elem.attribute("name");
      
      if(entities.find(j) == entities.end()) {
        Template *e = new Template(netcom);
        connect(e, &Template::setTemplate, this, &MainWindow::setTemplate);
        entities[j] = e;
        entities[j]->update(node);
        central->layout()->addWidget(e->widget());
        if(j == 0) e->widget()->setFocus();
      } else {
        entities[j]->update(node);
      }
      continue;
    }

    showError("XML Error", "Expected template tag. Got '" +
              elem.tagName() + "' tag.");
    continue;
  }
}

void MainWindow::setTemplate(QString name)
{
  templ = name;

  Entities::iterator i = entities.begin();
  while(i != entities.end()) {
    delete i.value()->widget();
    // delete i.value();
    i++;
  }

  entities.clear();
}

void MainWindow::handle(QDomDocument &doc)
{
  enableEntities(true);

  DEBUG(mainwindow, "Handle %s\n",
        doc.toString().toStdString().c_str());

  QDomNodeList nodes = doc.documentElement().childNodes();
  for(int j = 0; j < nodes.count(); j++) {
    QDomNode node = nodes.at(j);
    QDomElement element = node.toElement();

    if(element.tagName() == "error") {
      showError("Pracro Server Error", element.text());
      closing = false;
      setEnabled(true);
      patientid_label->setText(patientTitle);
      continue;
    }

    if(element.tagName() != "template" && element.tagName() != "course") {
      showError("XML error", "Outer tag not a template or course.");
      continue;
    }

    if(element.tagName() == "template" && element.attribute("name") != templ) {
      showError("XML error",
                "Got template name that didn't match current session.");
      continue;
    }

    // Set headers. titles, etc...
    updateTemplateHeaders(node);

    if(element.tagName() == "course") {
      // Update macros, header and resumes.
      QDomNodeList templatenodes = node.childNodes();
      updateTemplates(templatenodes);
    }

    if(element.tagName() == "template") {
      // Update macros, header and resumes.
      QDomNodeList macronodes = node.childNodes();
      updateMacros(macronodes);
    }

  }

  if(closing) {
    isStored = true;
    close();
  }
}

void MainWindow::checkFocus()
{
  if(!isActiveWindow()) {
    lostFocus = true;
  }
}
