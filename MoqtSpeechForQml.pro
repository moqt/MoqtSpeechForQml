TEMPLATE = lib
CONFIG += static plugin
CONFIG += c++11
CONFIG += qmltypes
QT += qml

QML_IMPORT_NAME = org.moqt.speech
QML_IMPORT_MAJOR_VERSION = 1
QMLTYPES_FILENAME = $$PWD/../qmlimports/org/moqt/speech/plugins.qmltypes

DESTDIR = $$PWD/../libs/$$first(QMAKE_PLATFORM)
TARGET = $$qtLibraryTarget($$TARGET)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    plugin.cpp \
    speechrecognizer.cpp \
    texttospeech.cpp

HEADERS += \
    speechrecognizer.h \
    speechrecognizeradapter.h \
    texttospeech.h \
    texttospeechadapter.h

DISTFILES += \
    Android/Java/org/moqt/speech/*.*


android {
    SOURCES += \
        Android/texttospeechandroid.cpp \
        Android/speechrecognizerandroid.cpp
}

ios {
    OBJECTIVE_SOURCES += \
        Ios/texttospeechios.mm \
        Ios/speechrecognizerios.mm
}

macx {
    SOURCES += \
        speechrecognizermock.cpp \
        texttospeechmock.cpp
}
