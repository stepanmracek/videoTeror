#ifndef MOTIONDETECTOR_H
#define MOTIONDETECTOR_H

#include "common.h"

#include <QList>
#include <deque>

namespace VideoTeror
{
namespace MotionDetection
{

class MotionDetector
{
protected:
    QList<GrayscaleImage> history;
    //std::deque<GrayscaleImage> history; //
    int historyLength;
    GrayscaleImage getMeanHistoryImage();

public:  
    virtual GrayscaleImage detect(const GrayscaleImage &curFrame) = 0;

    void morphClosure(GrayscaleImage &bitmap);
};

}
}

#endif // MOTIONDETECTOR_H
