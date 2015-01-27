#ifndef VT_TEMPLATEMATCHER_H
#define VT_TEMPLATEMATCHER_H

#include <opencv2/opencv.hpp>

#include "vtsense/vtsense.h"

namespace VideoTeror
{
namespace ObjectDetection
{

class TemplateMatcher
{
    std::vector<BGRImage> templates;
    std::vector<cv::Mat> resultMats;

public:
    struct Result
    {
        cv::Point location;
        double score;
    };

    TemplateMatcher(const std::vector<BGRImage> &templates);

    Result match(const BGRImage &frame);
};

}
}
#endif // VT_TEMPLATEMATCHER_H
