TEMPLATE = lib
CONFIG += static plugin
CONFIG += c++11
CONFIG += qmltypes
QT += qml

QML_IMPORT_NAME = org.moqt.speech
QML_IMPORT_MAJOR_VERSION = 1
QMLTYPES_FILENAME = $$PWD/qmlimports/org/moqt/speech/plugins.qmltypes

DESTDIR = $$PWD/libs/$$first(QMAKE_PLATFORM)
TARGET = $$qtLibraryTarget($$TARGET)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/sources # Generated cpp files need access to headers

SOURCES += \
    sources/plugin.cpp \
    sources/speechrecognizer.cpp \
    sources/texttospeech.cpp

HEADERS += \
    sources/speechrecognizer.h \
    sources/speechrecognizeradapter.h \
    sources/texttospeech.h \
    sources/texttospeechadapter.h

DISTFILES += \
    sources/android/java/org/moqt/speech/*.*


android {
    SOURCES += \
        sources/android/texttospeechandroid.cpp \
        sources/android/speechrecognizerandroid.cpp
}

ios {
    OBJECTIVE_SOURCES += \
        sources/ios/texttospeechios.mm \
        sources/ios/speechrecognizerios.mm
}

macx {
    SOURCES += \
        sources/speechrecognizermock.cpp \
        sources/texttospeechmock.cpp
}
