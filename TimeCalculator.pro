include(common.pri)

TEMPLATE = subdirs

application.file = src/app.pro
application.makefile = Makefile

SUBDIRS += \
		version \
		lib \
		application

CONFIG += ordered

