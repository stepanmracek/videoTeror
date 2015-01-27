#ifndef VT_MOTIONDETECTOR_H
#define VT_MOTIONDETECTOR_H

#include "vtsense/vtsense.h"

#include <deque>

namespace VideoTeror
{
namespace MotionDetection
{

class MotionDetector
{
protected:
    std::deque<GrayscaleImage> history;
    unsigned int historyLength;
    GrayscaleImage getMeanHistoryImage();

public:  
    typedef cv::Ptr<MotionDetector> Ptr;
    virtual GrayscaleImage detect(const GrayscaleImage &curFrame) = 0;
    virtual GrayscaleImage getForeground() { return GrayscaleImage(); }

    void morphClosure(GrayscaleImage &bitmap, int parameter);

    std::vector<cv::Rect> getBoundingRectangles(const VideoTeror::GrayscaleImage &detectionResult, int minArea, int maxArea);
};

}
}

#endif // VT_MOTIONDETECTOR_H
