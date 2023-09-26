/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            main.cc
 *
 *  Fri Jul 13 12:38:45 CEST 2007
 *  Copyright 2007 Bent Bisballe Nyeng and Lars Bisballe Jensen
 *  deva@aasimon.org and larsbisballe@gmail.com
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

#include "strtools.h"
#include "mainwindow.h"
#include "launcherwindow.h"
#include "patientidocr.h"
#include "debug.h"

#include <QApplication>
#include <QObject>
#include <QStringList>
#include <QSettings>
#include <QFileInfo>
#include <QMessageBox>
#include <QStyleFactory>
#include <QDir>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTranslator>
#include <QFontDatabase>
#include <QFont>
#include <QProcess>

#ifdef Q_OS_WIN
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <Lmcons.h>
#endif

#if defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
#include <unistd.h>
#endif

#ifdef Q_OS_WIN
bool onlyInstance()
{
  HANDLE hProcessSnap;
  PROCESSENTRY32 pe32;

  // Take a snapshot of all processes in the system.
  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if(hProcessSnap == INVALID_HANDLE_VALUE)
    return false;

  // Set the size of the structure before using it.
  pe32.dwSize = sizeof(PROCESSENTRY32);

  // Retrieve information about the first process,
  // and exit if unsuccessful
  if(!Process32First(hProcessSnap, &pe32))
    {
      CloseHandle(hProcessSnap); // clean the snapshot object
      return false;
    }

  int instances = 0;
  // Now walk the snapshot of processes, and
  // display information about each process in turn
  do
    {
      printf("Process: %S\n", pe32.szExeFile);
      if(!wcscmp(pe32.szExeFile, L"pracro.exe"))
        instances++;
    } while(Process32Next(hProcessSnap, &pe32));

  if(instances > 1)
    return false;
  
  CloseHandle(hProcessSnap);
  return true;
}
#endif

QString getCurrentUser()
{
  QString currentUser = "na";
#if defined(Q_OS_WIN)
  TCHAR name[UNLEN + 1];
  DWORD size = UNLEN + 1;

  if(GetUserName((TCHAR*)name, &size)) {
    currentUser = QString::fromWCharArray(name);
  }
#endif
#if defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
  char *name = getlogin();
  if(name != nullptr) {
    currentUser = QString::fromUtf8(name);
  }
#endif
  return currentUser;
}

QString patientId = "";
QString user = "";

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  app.setApplicationVersion(VERSION);
  app.setStyle(QStyleFactory::create("fusion"));

#ifdef Q_OS_WIN
  if(!onlyInstance()) {
    QMessageBox::critical(nullptr, "Pracro", "An instance of Pracro is already running.");
    return 0;
  }
#endif

  user = getCurrentUser();

  QCommandLineParser parser;

  parser.setApplicationDescription("Pracro is a graphical patient diagnostic tool designed to allow integration with diagnostic equipment and to ensure well-defined and structured endpoint patient data.");
  parser.addHelpOption();
  parser.addVersionOption();
  QCommandLineOption userOption(QStringList({{"U"}, {"user"} }), "Username that will be applied to committed macros.", "STRING", "");
  QCommandLineOption patientidOption(QStringList({{"P"}, {"patientid"}}), "Sets what patient-id to work on.", "STRING", "");
  QCommandLineOption runentryOption(QStringList({{"r"}, {"runentry"}}), "Executes the Pracro Launcher using a custom runentries entry. It can be set to either a runentries formatted file or with the format of a single line from a runentries formatted file. Will use the patient-id detected using OCR or set with '-P'.", "STRING", "");
  QCommandLineOption titleOption(QStringList({{"T"}, {"title"}}), "Sets the application window title.", "STRING", "");
  QCommandLineOption configOption(QStringList({{"c"}, {"config"}}), "Sets what overall config ini file to use. Default is 'pracro.ini'", "STRING", "");
  QCommandLineOption ocrConfigOption(QStringList({{"o"}, {"ocr"}}), "Sets what patient ID OCR config ini file to use. Default is 'ocr.ini'", "STRING", "");
  QCommandLineOption debugOption(QStringList({{"d"}, {"debug"}}), "Enables the togglable debug window in the user interface.");
  parser.addOption(userOption);
  parser.addOption(patientidOption);
  parser.addOption(runentryOption);
  parser.addOption(titleOption);
  parser.addOption(configOption);
  parser.addOption(ocrConfigOption);
  parser.addOption(debugOption);
  parser.process(app);
 
  // user is global
  // patientId is global
  QString runEntry = "";
  QString title = "";
  QString config = "pracro.ini";
  QString ocrConfig = "ocr.ini";
  QString host = "localhost";
  quint16 port = 12345;
  bool useHttps = false;
  bool certVerify = false;

  QDir::setCurrent(QApplication::applicationDirPath());

  if(parser.isSet(configOption)) {
    config = parser.value(configOption);
  }

  if(parser.isSet(ocrConfigOption)) {
    ocrConfig = parser.value(ocrConfigOption);
  }

  // If OCR config ini is not found, use overall config instead
  if(!QFileInfo::exists(ocrConfig)) {
    ocrConfig = config;
  }

  QSettings settings(config, QSettings::IniFormat);
  QSettings ocrSettings(ocrConfig, QSettings::IniFormat);

  // Main specific
  if(settings.contains("main/user")) {
    user = settings.value("main/user").toString();
  }

  if(!settings.contains("main/runentries")) {
    settings.setValue("main/runentries", "data/runentries.dat");
  }
  runEntry = settings.value("main/runentries", "data/runentries.dat").toString();
  
  // Patient id OCR specific
  if(!ocrSettings.contains("patientidocr/enabled")) {
    ocrSettings.setValue("patientidocr/enabled", true);
  }

  if(!ocrSettings.contains("patientidocr/anchorxdelta")) {
    ocrSettings.setValue("patientidocr/anchorxdelta", 42);
  }

  if(!ocrSettings.contains("patientidocr/anchorydelta")) {
    ocrSettings.setValue("patientidocr/anchorydelta", 12);
  }

  if(!ocrSettings.contains("patientidocr/maxcolordifference")) {
    ocrSettings.setValue("patientidocr/maxcolordifference", 80);
  }

  if(!ocrSettings.contains("patientidocr/debug")) {
    ocrSettings.setValue("patientidocr/debug", false);
  }

  // Server specific
  if(!settings.contains("server/host")) {
    settings.setValue("server/host", "localhost");
  }
  host = settings.value("server/host").toString();

  if(!settings.contains("server/port")) {
    settings.setValue("server/port", 12345);
  }
  port = settings.value("server/port").toInt();

  if(!settings.contains("server/usehttps")) {
    settings.setValue("server/usehttps", true);
  }
  useHttps = settings.value("server/usehttps").toBool();

  if(!settings.contains("server/certverify")) {
    settings.setValue("server/certverify", false);
  }
  certVerify = settings.value("server/certverify").toBool();

  // Check command-line, these override ini settings
  if(parser.isSet(userOption)) {
    user = parser.value(userOption);
  }
  if(parser.isSet(patientidOption)) {
    patientId = parser.value(patientidOption);
  }
  if(parser.isSet(runentryOption)) {
    runEntry = parser.value(runentryOption);
  }
  if(parser.isSet(titleOption)) {
    title = parser.value(titleOption);
  }
  if(parser.isSet(debugOption)) {
    dbg_init();
  }

  // Handle / parse runentrie(s)
  QList<QString> runEntries;
  if(QFileInfo::exists(runEntry)) {
    QFile runEntriesFile(runEntry);
    if(runEntriesFile.open(QIODevice::ReadOnly)) {
      while(!runEntriesFile.atEnd()) {
        QString line = QString::fromUtf8(runEntriesFile.readLine().simplified());
        if(line.left(1) == "#" || line.isEmpty()) {
          continue;
        }
        runEntries.append(line);
      }
    }
  } else if((runEntry.count(";") == 1 || runEntry.count(";") == 2) &&
            (runEntry.contains("launch") || runEntry.contains("course"))) {
    runEntries.append(runEntry);
  }
  if(runEntries.isEmpty()) {
    printf("No runentries found. Please set one or more by setting '--runentry' to either a single runentry line or a file containing runentry entries.\n");
    return 1;
  }

  QTranslator translator;
  if(translator.load("pracro_dk")) {
    app.installTranslator(&translator);
  }

  QFontDatabase::addApplicationFont(":VeraMono.ttf");

  if(ocrSettings.value("patientidocr/enabled").toBool() &&
     patientId.isEmpty()) {
    PatientIdOcr patientIdOcr(ocrSettings.value("patientidocr/anchorxdelta").toInt(),
                              ocrSettings.value("patientidocr/anchorydelta").toInt(),
                              ocrSettings.value("patientidocr/maxcolordifference").toInt(),
                              ocrSettings.value("patientidocr/debug").toBool());
    patientId = patientIdOcr.getPatientId();
  }

  if(!patientId.isEmpty() && patientId.length() == 10 && !patientId.contains("-")) {
    patientId = patientId.left(6) + "-" + patientId.right(4);
  }

  if((!StrTools::isPatientId(patientId) ||
      patientId.isEmpty()) ||
     runEntries.length() > 1) {
    LauncherWindow launcher(patientId, runEntries);
    if(launcher.exec() == QDialog::Accepted && !launcher.getPatientId().isEmpty()) {
      patientId = launcher.getPatientId();
      runEntries.clear();
      runEntries.append(launcher.getRunEntry());
    } else {
      return 0;
    }
  }
  QList<QString> snippets = runEntries.first().split(';');
  QString type = snippets.at(0);
  QString value = snippets.at(1);
  if(type == "launch") {
    QString commandPatientId = patientId;
    value.replace("%P", commandPatientId);
    commandPatientId.replace("-", "");
    value.replace("%p", commandPatientId);
    QList<QString> commandAndArguments = value.split(' ');
    if(commandAndArguments.length() >= 1) {
      QProcess::startDetached(commandAndArguments.at(0), commandAndArguments.mid(1));
    }
    return 0;
  }

  // Set title to patient-id if nothing else has set it
  if(title.isEmpty()) {
    title = patientId;
  }

  if(patientId.contains("-")) {
    patientId.replace("-", "");
  }

  MainWindow mainwindow(patientId, title, value, host, port, useHttps, certVerify, user);
  mainwindow.show();

  int ret = app.exec();

  return ret;
}
