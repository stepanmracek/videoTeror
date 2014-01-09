#ifndef BACKGROUNDSUBTRACTORDETECTOR_H
#define BACKGROUNDSUBTRACTORDETECTOR_H

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

public:
    BackgroundSubtractorDetector();

    GrayscaleImage detect(const GrayscaleImage &curFrame);
};

}
}

#endif // BACKGROUNDSUBTRACTORDETECTOR_H
