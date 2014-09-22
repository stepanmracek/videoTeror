QT += core gui

TARGET = vtsense

TEMPLATE = lib

include("../common.pri")

SOURCES += \
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
    Helpers/serialization.cpp \
    Helpers/helpers.cpp \
    MachineLearning/probabilisticmodel.cpp \
    MachineLearning/svm.cpp

HEADERS += \
    MotionDetection/motiondetector.h \
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
    Helpers/serialization.h \
    Helpers/helpers.h \
    MachineLearning/probabilisticmodel.h \
    MachineLearning/binaryclassifier.h \
    MachineLearning/svm.h \
    vtsense.h
