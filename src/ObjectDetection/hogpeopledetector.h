#ifndef HOGPEOPLEDETECTOR_H
#define HOGPEOPLEDETECTOR_H

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
    HogPeopleDetector(double inputFrameScale, double threshold);

    virtual DetectionResult detect(const BGRImage &curFrame);
};

}
}

#endif // HOGPEOPLEDETECTOR_H
