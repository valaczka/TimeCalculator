include(../common.pri)

TEMPLATE = subdirs

SUBDIRS += \
	libQaterial \
	libQtXlsxWriter


!wasm: SUBDIRS += CuteLogger




