#ifndef VT_ANOTATION_H
#define VT_ANOTATION_H

#include <opencv2/opencv.hpp>
#include <vector>

#include "vtsense/vtsense.h"

namespace VideoTeror
{
namespace Helpers
{

class Anotation
{
private:
    static void onMouseRegionSelect(int event, int x, int y, int flags, void* userdata);
    static void onMousePointSelect(int event, int x, int y, int flags, void* userdata);

public:
    struct AnotationData
    {
        enum State
        {
            Waiting,
            FirstClick,
            SecondClick
        };

        State state;
        cv::Point firstPoint;
        cv::Point secondPoint;
        BGRImage currentFrame;
        std::vector<BGRImage> result;

        AnotationData();
        void draw();
    };

    static std::vector<BGRImage> anotateRegions(cv::VideoCapture &video);

    static std::vector<cv::Point2f> anotatePoints(const BGRImage &frame);

    static void extractFeatures(const std::vector<BGRImage> &textures, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors);

};

}
}

#endif // VT_ANOTATION_H
