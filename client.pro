# -*- Makefile -*-
CONFIG += release
unix:CONFIG += link_pkgconfig
TEMPLATE = app
TARGET = pracro
DEPENDPATH += . src
INCLUDEPATH += . src
RESOURCES += client.qrc
QT += core gui network xml widgets
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp
unix:PKGCONFIG += lua51

TRANSLATIONS = pracro_dk.ts

# For debugging
debug {
	QMAKE_CXXFLAGS += -g -Wall -Werror
	CONFIG += debug
	DEFINES += USE_DEBUG
}

include(VERSION)
DEFINES+=VERSION=\\\"$$VERSION\\\"

win32 {
  QMAKE_LIBDIR += lua51/lib
  INCLUDEPATH += lua51/include
  LIBS += -llua5.1
  DEFINES += HOST_WIN32
#	debug {
	  CONFIG +=
#	}
}

#unix:INCLUDEPATH += /usr/include/lua5.1
#unix:LIBS += -llua5.1

HEADERS += \
	src/collapser.h \
	src/debug.h \
	src/entity.h \
	src/header.h \
	src/strtools.h \
	src/launcherwindow.h \
	src/patientidocr.h \
	src/lua.h \
	src/luawidget.h \
	src/macro.h \
	src/macrowindow.h \
	src/mainwindow.h \
	src/macrodrawer.h \
	src/messagebox.h \
	src/netcom.h \
	src/resumewidget.h \
	src/template.h \
	src/shortcuttooltip.h \
	src/widgets.h \
	src/expandbutton.h \
	src/aboutbox.h \
	src/widgets/common.h \
	src/widgets/widget.h \
	src/widgets/label.h \
	src/widgets/lineedit.h \
	src/widgets/multilist.h \
	src/widgets/textedit.h \
	src/widgets/button.h \
	src/widgets/datetime.h \
	src/widgets/combobox.h \
	src/widgets/listbox.h \
	src/widgets/frame.h \
	src/widgets/groupbox.h \
	src/widgets/radiobutton.h \
	src/widgets/radiobuttons.h \
	src/widgets/checkbox.h \
	src/widgets/checkgroupbox.h \
	src/widgets/window.h \
	src/widgets/altcombobox.h \
	src/widgets/metawidget.h

SOURCES += \
	src/pracro.cc \
	src/collapser.cc \
	src/debug.cc \
	src/header.cc \
	src/strtools.cc \
	src/launcherwindow.cc \
	src/patientidocr.cc \
	src/lua.cc \
	src/luawidget.cc \
	src/macro.cc \
	src/macrowindow.cc \
	src/mainwindow.cc \
	src/macrodrawer.cc \
	src/messagebox.cc \
	src/netcom.cc \
	src/resumewidget.cc \
	src/template.cc \
	src/shortcuttooltip.cc \
	src/expandbutton.cc \
	src/aboutbox.cc \
	src/widgets/common.cc \
	src/widgets/widget.cc \
	src/widgets/label.cc \
	src/widgets/lineedit.cc \
	src/widgets/multilist.cc \
	src/widgets/textedit.cc \
	src/widgets/button.cc \
	src/widgets/combobox.cc \
	src/widgets/datetime.cc \
	src/widgets/listbox.cc \
	src/widgets/frame.cc \
	src/widgets/groupbox.cc \
	src/widgets/radiobutton.cc \
	src/widgets/radiobuttons.cc \
	src/widgets/checkbox.cc \
	src/widgets/checkgroupbox.cc \
	src/widgets/window.cc \
	src/widgets/altcombobox.cc \
	src/widgets/metawidget.cc

RC_FILE = client.rc
