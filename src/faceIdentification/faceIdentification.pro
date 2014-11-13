QT += core gui widgets
TARGET = faceIdentification
TEMPLATE = app

include("../common.pri")
include("../vtsense.pri")
include("../faceCommon.pri")

SOURCES += \
    main.cpp
