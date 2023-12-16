lessThan(QT_MAJOR_VERSION, 6): error(Minimum Qt version 6 required)

TEMPLATE = app
TARGET = test-sql

QT += gui quick svg quickcontrols2 sql

CONFIG += c++17
CONFIG += separate_debug_info

include(../common.pri)
include(../version/version.pri)
#include(../../translations/translations.pri)

DESTDIR = ..

include(../lib/import_lib.pri)

!android:if(linux|win32){
	QMAKE_LFLAGS += \
		"-Wl,--rpath,'$${LITERAL_DOLLAR}$${LITERAL_DOLLAR}ORIGIN'" \
		"-Wl,--rpath,'$${LITERAL_DOLLAR}$${LITERAL_DOLLAR}ORIGIN/lib'"
}


SOURCES += \
	abstractapplication.cpp \
	application.cpp \
	desktopapplication.cpp \
	main.cpp \
	onlineapplication.cpp \
	utils_.cpp

HEADERS += \
	../version/version.h \
	abstractapplication.h \
	application.h \
	desktopapplication.h \
	onlineapplication.h \
	querybuilder.hpp \
	utils_.h

RESOURCES += \
	../qml/qml.qrc \
	../qml/QaterialHelper.qrc \
	../share/share.qrc
