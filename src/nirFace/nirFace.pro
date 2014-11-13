QT += core gui widgets
TARGET = nirFace
TEMPLATE = app

include("../common.pri")
include("../vtsense.pri")

LIBS += -lOpenNI2

SOURCES += \
    main.cpp
