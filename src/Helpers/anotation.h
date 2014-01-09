#ifndef ANOTATION_H
#define ANOTATION_H

#include <QVector>
#include <opencv2/opencv.hpp>
#include <vector>

#include "common.h"

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
        QVector<BGRImage> result;

        AnotationData();
        void draw();
    };

    static QVector<BGRImage> anotateRegions(cv::VideoCapture &video);

    static std::vector<cv::Point2f> anotatePoints(const BGRImage &frame);

    static void extractFeatures(const QVector<BGRImage> &textures, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors);

};

}
}

#endif // ANOTATION_H
