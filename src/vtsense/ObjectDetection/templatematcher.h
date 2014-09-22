#ifndef VT_TEMPLATEMATCHER_H
#define VT_TEMPLATEMATCHER_H

#include <QVector>
#include <opencv2/opencv.hpp>

#include "vtsense.h"

namespace VideoTeror
{
namespace ObjectDetection
{

class TemplateMatcher
{
    QVector<BGRImage> templates;
    QVector<cv::Mat> resultMats;

public:
    struct Result
    {
        cv::Point location;
        double score;
    };

    TemplateMatcher(const QVector<BGRImage> &templates);

    Result match(const BGRImage &frame);
};

}
}
#endif // VT_TEMPLATEMATCHER_H
