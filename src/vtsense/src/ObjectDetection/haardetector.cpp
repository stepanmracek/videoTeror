#include "vtsense/ObjectDetection/haardetector.h"

#include <stdexcept>

using namespace VideoTeror::ObjectDetection;

HaarDetector::HaarDetector(const std::string &path, double scaleFactor,
                           cv::Size minSize, cv::Size maxSize)
    : scaleFactor(scaleFactor),
      minSize(minSize),
      maxSize(maxSize)
{
    if (!classifier.load(path))
        throw std::runtime_error("Could not load classifier: " + path);
}

std::vector<ObjectDetector::DetectionResult> HaarDetector::detect(const BGRImage &curFrame)
{
    std::vector<ObjectDetector::DetectionResult> result;

    cv::cvtColor(curFrame, frame, CV_BGR2GRAY);
    cv::resize(frame, resizedFrame, cv::Size(frame.cols*scaleFactor, frame.rows*scaleFactor));

    std::vector<cv::Rect> objects;
    classifier.detectMultiScale(resizedFrame, objects, 1.2, 2, 0 | CV_HAAR_SCALE_IMAGE, minSize, maxSize);

    int id = 0;
    for (const cv::Rect &o : objects)
    {
        ObjectDetector::DetectionResult r;
        r.id = id;
        r.point = cv::Point2d(((double)o.x + o.width/2) / resizedFrame.cols,
                              ((double)o.y + o.height/2) / resizedFrame.rows);
        r.region = cv::Rect_<double>(((double)o.x) / resizedFrame.cols,
                                     ((double)o.y) / resizedFrame.rows,
                                     ((double)o.width) / resizedFrame.cols,
                                     ((double)o.height) / resizedFrame.rows);
        r.score = 1.0;
        result.push_back(r);

        id++;
    }

    return result;
}
