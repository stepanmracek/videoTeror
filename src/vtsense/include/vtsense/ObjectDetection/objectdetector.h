#ifndef VT_OBJECTDETECTOR_H
#define VT_OBJECTDETECTOR_H

#include <opencv2/opencv.hpp>

#include "vtsense/vtsense.h"

namespace VideoTeror
{
namespace ObjectDetection
{

class ObjectDetector
{
public:

    struct DetectionResult
    {
        std::vector<cv::Rect> objects;
        std::vector<double> scores;
    };

    virtual DetectionResult detect(const BGRImage &curFrame) = 0;
};

}
}
#endif // VT_OBJECTDETECTOR_H
