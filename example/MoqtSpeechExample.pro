QT += quick

SOURCES += \
        main.cpp

resources.files = qtquickcontrols2.conf \
                  main.qml
RESOURCES += resources

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$PWD/../qmlimports

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Link MoqtSpeechForQml library
LIBS += -L$$PWD/../libs/$$first(QMAKE_PLATFORM)
LIBS += -l$$qtLibraryTarget(MoqtSpeechForQml)


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


DISTFILES += \
    android/*.* \
    android/gradle/*.* \
    android/res/values/*.*

android {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
