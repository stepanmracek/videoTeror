QT += core gui widgets
TARGET = faceDetection
TEMPLATE = app

include("../common.pri")
include("../vtsense.pri")
include("../faceCommon.pri")

SOURCES += \
    main.cpp
