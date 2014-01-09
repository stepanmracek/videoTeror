#ifndef OBJECTDETECTOR_H
#define OBJECTDETECTOR_H

#include <QVector>
#include <opencv2/opencv.hpp>

#include "common.h"

namespace VideoTeror
{
namespace ObjectDetection
{

class ObjectDetector
{
public:

    struct DetectionResult
    {
        QVector<cv::Rect> objects;
        QVector<double> scores;
    };

    virtual DetectionResult detect(const BGRImage &curFrame) = 0;
};

}
}
#endif // OBJECTDETECTOR_H
