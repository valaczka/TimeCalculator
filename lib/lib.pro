include(../common.pri)

TEMPLATE = subdirs

SUBDIRS += \
	libQaterial \


!wasm: SUBDIRS += CuteLogger




