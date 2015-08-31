#include "vtsense/ObjectDetection/hogpeopledetector.h"

using namespace VideoTeror::ObjectDetection;

HogPeopleDetector::HogPeopleDetector(double inputFrameScale, double threshold) :
    inputFrameScale(inputFrameScale),
    threshold(threshold)
{
    hog.winSize = cv::Size(48, 96);
    hog.setSVMDetector(cv::HOGDescriptor::getDaimlerPeopleDetector());
    //hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
    locations.reserve(50);
}

std::vector<ObjectDetector::DetectionResult> HogPeopleDetector::detect(const BGRImage &curFrame)
{
    std::vector<ObjectDetector::DetectionResult> result;

    std::vector<double> weights;
    cv::resize(curFrame, frame, cv::Size(curFrame.cols*inputFrameScale, curFrame.rows*inputFrameScale));
    hog.detectMultiScale(frame, locations, weights, 0, cv::Size(8, 8), cv::Size(32, 32), 1.2, threshold);

    //double invScale = 1.0/inputFrameScale;
    for (unsigned int i = 0; i < locations.size(); i++)
    {
        if (weights[i] < threshold) continue;
        const cv::Rect &l = locations[i];

        ObjectDetector::DetectionResult detection;
        detection.id = i;
        detection.score  = weights[i];
        detection.region = cv::Rect_<double>(((double)l.x)/frame.cols, ((double)l.y)/frame.rows,
                                             ((double)l.width)/frame.cols, ((double)l.height)/frame.rows);
        detection.point = cv::Point2d(detection.region.x + detection.region.width*0.5,
                                      detection.region.y + detection.region.height*0.33);

        result.push_back(detection);
    }
    return result;
}
