#ifndef VT_HAARDETECTOR_H
#define VT_HAARDETECTOR_H

#include <opencv2/opencv.hpp>

#include "objectdetector.h"

namespace VideoTeror
{
namespace ObjectDetection
{

class HaarDetector : public ObjectDetector
{
private:
    cv::CascadeClassifier classifier;
    GrayscaleImage frame;
    GrayscaleImage resizedFrame;
    double scaleFactor;
    cv::Size minSize;
    cv::Size maxSize;

public:
    HaarDetector(const std::string &path, double scaleFactor,
                 cv::Size minSize = cv::Size(), cv::Size maxSize = cv::Size());

    virtual std::vector<ObjectDetector::DetectionResult> detect(const BGRImage &curFrame);
};

}
}

#endif //VT_HAARDETECTOR_H

