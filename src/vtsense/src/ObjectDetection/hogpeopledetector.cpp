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

ObjectDetector::DetectionResult HogPeopleDetector::detect(const BGRImage &curFrame)
{
    ObjectDetector::DetectionResult result;

    std::vector<double> weights;
    cv::resize(curFrame, frame, cv::Size(curFrame.cols*inputFrameScale, curFrame.rows*inputFrameScale));
    hog.detectMultiScale(frame, locations, weights, 0, cv::Size(8, 8), cv::Size(32, 32), 1.2, threshold); //1.2

    double invScale = 1.0/inputFrameScale;
    for (unsigned int i = 0; i < locations.size(); i++)
    {
        double score = weights[i];
        if (score < threshold) continue;
        cv::Rect r = locations[i];
        result.objects.push_back(cv::Rect(r.x*invScale, r.y*invScale, r.width*invScale, r.height*invScale));
        result.scores = weights;
    }
    return result;
}
