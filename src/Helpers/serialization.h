#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <QVector>
#include <opencv2/opencv.hpp>
#include <vector>

#include "common.h"

namespace VideoTeror
{
namespace Helpers
{

class Serialization
{
public:
    static void serialize(const std::string &filename, const std::vector<cv::KeyPoint> &keypoints, const cv::Mat &descriptors);
    static void deserialize(const std::string &filename, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors);

    static void serialize(const std::string &filename, const QVector<VideoTeror::BGRImage> &images);
    static void deserialize(const std::string &filename, QVector<VideoTeror::BGRImage> &images);
};

}
}

#endif // SERIALIZATION_H
