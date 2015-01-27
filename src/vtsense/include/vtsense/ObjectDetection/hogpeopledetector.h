#ifndef VT_HOGPEOPLEDETECTOR_H
#define VT_HOGPEOPLEDETECTOR_H

#include <opencv2/opencv.hpp>

#include "objectdetector.h"

namespace VideoTeror
{
namespace ObjectDetection
{

class HogPeopleDetector : public ObjectDetector
{
private:
    double inputFrameScale;
    double threshold;
    BGRImage frame;
    cv::HOGDescriptor hog;
    std::vector<cv::Rect> locations;

public:
    HogPeopleDetector(double inputFrameScale = 0.5, double threshold = 3.0);

    virtual DetectionResult detect(const BGRImage &curFrame);
};

}
}

#endif // VT_HOGPEOPLEDETECTOR_H
