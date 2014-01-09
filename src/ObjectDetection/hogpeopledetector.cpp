#include "hogpeopledetector.h"

#include <QVector>
#include <QDebug>

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
    hog.detectMultiScale(frame, locations, weights, 0, cv::Size(8, 8), cv::Size(32, 32), 2, threshold);

    double invScale = 1.0/inputFrameScale;
    foreach (const cv::Rect &r, locations)
    {
        result.objects << cv::Rect(r.x*invScale, r.y*invScale, r.width*invScale, r.height*invScale);
        result.scores = QVector<double>::fromStdVector(weights);
    }
    return result;
}
