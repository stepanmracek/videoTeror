QT += core gui widgets
TARGET = faceDetectionInVideo
TEMPLATE = app

include("../common.pri")
include("../vtsense.pri")
include("../faceCommon.pri")

SOURCES += \
    main.cpp
