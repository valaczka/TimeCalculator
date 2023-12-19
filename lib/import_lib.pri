include(../common.pri)

# Core

LIBS += -L../lib


# Qaterial + QOlm

INCLUDEPATH += \
	$$PWD/Qaterial/src \
	$$PWD/QOlm/include

QMLPATHS += $$PWD/Qaterial/qml/Qaterial

android: LIBS += -lQaterial_$${QT_ARCH}
else: LIBS += -lQaterial

DEFINES += QATERIAL_STATIC QOLM_STATIC


# CuteLogger

android|ios|wasm {
	HEADERS += $$PWD/../src/wasm_helper/Logger.h
	INCLUDEPATH += $$PWD/../src/wasm_helper
} else {
	INCLUDEPATH += $$PWD/CuteLogger/include
	LIBS += -lCuteLogger
}


# QtXlsxWriter

INCLUDEPATH += $$PWD/QtXlsxWriter/src/xlsx

android: LIBS += -lQtXlsxWriter_$${QT_ARCH}
else: LIBS += -lQtXlsxWriter



# QSyncable

include(qsyncable.pri)


# Emscripten-browser-file

wasm {
	INCLUDEPATH += $$PWD/emscripten-browser-file

	LIBS += -sEXPORTED_RUNTIME_METHODS=[ccall,specialHTMLTargets] -sALLOW_MEMORY_GROWTH=1 -sEXPORTED_FUNCTIONS=[_main,_malloc,_free]
}
