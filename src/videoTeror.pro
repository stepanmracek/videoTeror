#-------------------------------------------------
#
# Project created by QtCreator 2013-10-10T19:51:52
#
#-------------------------------------------------

QT += core gui

TARGET = videoTeror

TEMPLATE = app

LIBS += `pkg-config --libs opencv`

SOURCES += main.cpp \
    MotionDetection/motiondetector.cpp \
    MotionDetection/simplemotiondetector.cpp \
    MotionDetection/opticalflowmotiondetector.cpp \
    MotionDetection/farnebackmotiondetector.cpp \
    MotionDetection/gradientmotiondetector.cpp \
    MotionDetection/backgroundsubtractordetector.cpp \
    ObjectDetection/objectdetector.cpp \
    ObjectDetection/hogpeopledetector.cpp \
    Helpers/anotation.cpp \
    ObjectDetection/templatematcher.cpp \
    Tracking/tracker.cpp \
    Tracking/pyrlktracker.cpp \
    Helpers/serialization.cpp

HEADERS += \
    MotionDetection/motiondetector.h \
    common.h \
    MotionDetection/simplemotiondetector.h \
    MotionDetection/opticalflowmotiondetector.h \
    MotionDetection/farnebackmotiondetector.h \
    MotionDetection/gradientmotiondetector.h \
    MotionDetection/backgroundsubtractordetector.h \
    ObjectDetection/objectdetector.h \
    ObjectDetection/hogpeopledetector.h \
    Helpers/anotation.h \
    ObjectDetection/templatematcher.h \
    Tracking/tracker.h \
    Tracking/pyrlktracker.h \
    Helpers/serialization.h
