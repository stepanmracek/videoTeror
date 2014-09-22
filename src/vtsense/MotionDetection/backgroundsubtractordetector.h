#ifndef VT_BACKGROUNDSUBTRACTORDETECTOR_H
#define VT_BACKGROUNDSUBTRACTORDETECTOR_H

#include <opencv2/opencv.hpp>

#include "motiondetector.h"

namespace VideoTeror
{
namespace MotionDetection
{

class BackgroundSubtractorDetector : public MotionDetector
{
private:
    GrayscaleImage foreground;
    cv::BackgroundSubtractorMOG2 bs;
    int morphClosureParam;
    int scaleParam;

public:
    BackgroundSubtractorDetector(int morphClosureParam, int scaleParam);

    GrayscaleImage detect(const GrayscaleImage &curFrame);
};

}
}

#endif // VT_BACKGROUNDSUBTRACTORDETECTOR_H
