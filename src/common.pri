LIBS += `pkg-config --libs opencv`

QMAKE_CXXFLAGS += -std=c++11

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS +=  -fopenmp
