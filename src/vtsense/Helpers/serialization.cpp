#include "serialization.h"

using namespace VideoTeror::Helpers;

void Serialization::serialize(const std::string &filename, const std::vector<cv::KeyPoint> &keypoints, const cv::Mat &descriptors)
{
    cv::FileStorage storage(filename, cv::FileStorage::WRITE);
    storage << "keypoints" << "[";
    foreach (const cv::KeyPoint &keyPoint, keypoints)
    {
        storage << "{";
        storage << "pt" << keyPoint.pt << "size" << keyPoint.size << "angle" << keyPoint.angle
                << "class_id" << keyPoint.class_id << "octave" << keyPoint.octave << "response" << keyPoint.response;
        storage << "}";
    }
    storage << "]";
    storage << "descriptors" << descriptors;
}

void Serialization::deserialize(const std::string &filename, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors)
{
    keypoints.clear();

    cv::FileStorage storage(filename, cv::FileStorage::READ);
    cv::FileNode keypointsNode = storage["keypoints"];
    for (cv::FileNodeIterator nodeIterator = keypointsNode.begin(); nodeIterator != keypointsNode.end(); ++nodeIterator)
    {
        cv::FileNode keypointNode = *nodeIterator;

        cv::KeyPoint keypoint;
        keypointNode["pt"] >> keypoint.pt;
        keypointNode["size"] >> keypoint.size;
        keypointNode["angle"] >> keypoint.angle;
        keypointNode["class_id"] >> keypoint.class_id;
        keypointNode["octave"] >> keypoint.octave;
        keypointNode["response"] >> keypoint.response;

        keypoints.push_back(keypoint);
    }

    storage["descriptors"] >> descriptors;
}

void Serialization::serialize(const std::string &filename, const QVector<VideoTeror::BGRImage> &images)
{
    cv::FileStorage storage(filename, cv::FileStorage::WRITE);
    storage << "images" << "[";

    foreach (const VideoTeror::BGRImage &img, images)
    {
        storage << img;
    }

    storage << "]";
}

void Serialization::deserialize(const std::string &filename, QVector<VideoTeror::BGRImage> &images)
{
    images.clear();

    cv::FileStorage storage(filename, cv::FileStorage::READ);
    cv::FileNode imagesNode = storage["images"];
    for (cv::FileNodeIterator it = imagesNode.begin(); it != imagesNode.end(); ++it)
    {
        VideoTeror::BGRImage img;
        (*it) >> img;
        images << img;
    }
}
