QT += core gui widgets
TARGET = faceIdentification
TEMPLATE = app

include("../common.pri")
include("../vtsense.pri")

LIBS += -L/home/stepo/build/face/release-qt5/faceCommon -lfaceCommon
INCLUDEPATH += /home/stepo/git/tbs-devices/3D_Face/facelib/src/faceCommon

SOURCES += \
    main.cpp
