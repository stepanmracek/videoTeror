#ifndef VT_MOTIONDETECTOR_H
#define VT_MOTIONDETECTOR_H

#include "vtsense.h"

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
    typedef cv::Ptr<MotionDetector> Ptr;
    virtual GrayscaleImage detect(const GrayscaleImage &curFrame) = 0;

    void morphClosure(GrayscaleImage &bitmap, int parameter);
};

}
}

#endif // VT_MOTIONDETECTOR_H
