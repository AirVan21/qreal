QT += widgets network

CONFIG += c++11

TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$PWD/../../../../../bin/plugins/tools/kitPlugins/
TARGET = robots-trik-ruc-generator

MOC_DIR = .moc
RCC_DIR = .moc
OBJECTS_DIR = .obj

LIBS += -L$$PWD/../../../../../bin -lqrkernel -lqslog -lqrutils -lqrrepo -lqscintilla2 \
                -lrobots-generator-base -lrobots-trik-generator-base -lrobots-utils \

INCLUDEPATH += \
        $$PWD/../trikGeneratorBase/include/ \
        $$PWD/../../generatorBase/include/ \
        $$PWD/../../../interpreters/interpreterBase/include \
        $$PWD/../../../utils/include/ \
        $$PWD/../../../../../ \
        $$PWD/../../../../../qrgui \
        $$PWD/../../../../../qrtext/include \

# workaround for http://bugreports.qt.nokia.com/browse/QTBUG-8110
# when fixed it would become possible to use QMAKE_LFLAGS_RPATH
!macx {
        QMAKE_LFLAGS += -Wl,-O1,-rpath,$$PWD/../../../../../bin/
        QMAKE_LFLAGS += -Wl,-rpath,$$PWD/../../../../../bin/plugins/
}

#TRANSLATIONS = $$PWD/../../../../../qrtranslations/ru/plugins/robots/trikQtsGenerator_ru.ts

HEADERS += \
        $$PWD/trikRuCGeneratorPlugin.h \
        $$PWD/trikRuCMasterGenerator.h


SOURCES += \
        $$PWD/trikRuCGeneratorPlugin.cpp \
        $$PWD/trikRuCMasterGenerator.cpp


#RESOURCES = \
#       $$PWD/trikRuCGenerator.qrc \
#        $$PWD/templates.qrc \

RESOURCES += \
    $$PWD/trikRuCGenerator.qrc \
    templates.qrc
