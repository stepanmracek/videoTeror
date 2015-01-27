#include "vtsense/Helpers/anotation.h"

#include <vector>
#include <iostream>

using namespace VideoTeror::Helpers;

Anotation::AnotationData::AnotationData() :
    state(Waiting)
{
}

void Anotation::AnotationData::draw()
{
    BGRImage copy;
    currentFrame.copyTo(copy);

    switch (state)
    {
    case FirstClick:
        cv::circle(copy, firstPoint, 2, cv::Scalar(0, 255, 255), 1, CV_AA);
        break;
    case SecondClick:
        cv::rectangle(copy, firstPoint, secondPoint, cv::Scalar(0, 255, 255), 1);
        break;
    }

    cv::imshow("texture anotation", copy);
}

void Anotation::onMouseRegionSelect(int event, int x, int y, int flags, void *userdata)
{
    if (event != cv::EVENT_LBUTTONUP) return;

    AnotationData *data = (AnotationData*)userdata;

    cv::Point p(x, y);
    switch (data->state)
    {
    case Anotation::AnotationData::Waiting:
        data->firstPoint = p;
        data->state = Anotation::AnotationData::FirstClick;
        break;
    case Anotation::AnotationData::FirstClick:
        data->secondPoint = p;
        data->state = Anotation::AnotationData::SecondClick;
        break;
    case Anotation::AnotationData::SecondClick:
        cv::Rect rect(data->firstPoint, data->secondPoint);
        if (rect.contains(p))
        {
            data->result.push_back(data->currentFrame(rect).clone());
            std::cout << "added" << std::endl;
        }
        data->state = Anotation::AnotationData::Waiting;
    }

    data->draw();
}

std::vector<VideoTeror::BGRImage> Anotation::anotateRegions(cv::VideoCapture &video)
{
    AnotationData data;
    cv::namedWindow("texture anotation");
    cv::setMouseCallback("texture anotation", onMouseRegionSelect, &data);

    VideoTeror::BGRImage frame;
    while (video.read(frame))
    {
        data.currentFrame = frame;
        data.state = Anotation::AnotationData::Waiting;
        data.draw();
        if ((char)cv::waitKey() == 27) break;
    }

    cv::destroyWindow("texture anotation");
    return data.result;
}

void Anotation::onMousePointSelect(int event, int x, int y, int flags, void *userdata)
{
    if (event != cv::EVENT_LBUTTONUP) return;

    std::vector<cv::Point2f> *points = (std::vector<cv::Point2f>*)userdata;
    points->push_back(cv::Point2f(x, y));
    std::cout << "adding " << x << " " << y << std::endl;
}

std::vector<cv::Point2f> Anotation::anotatePoints(const BGRImage &frame)
{
    std::vector<cv::Point2f> result;
    cv::namedWindow("point anotation");
    cv::setMouseCallback("point anotation", onMousePointSelect, &result);
    cv::imshow("point anotation", frame);
    cv::waitKey();
    cv::destroyWindow("point anotation");
    return result;
}

void Anotation::extractFeatures(const std::vector<BGRImage> &textures, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors)
{
    cv::ORB featureExtractor;
    keypoints.clear();
    unsigned int n = textures.size();
    std::vector<cv::Mat> descriptorsPerTexture(n);
    int r = 0;
    for (unsigned int i = 0; i < n; i++)
    {
        std::vector<cv::KeyPoint> kp;
        featureExtractor(textures[i], cv::Mat(), kp, descriptorsPerTexture[i]);
        r += descriptorsPerTexture[i].rows;

        std::cout << kp.size() << std::endl;

        unsigned int m = kp.size();
        for (unsigned int j = 0; j < m; j++)
            keypoints.push_back(kp[j]);
    }

    descriptors = cv::Mat(r, featureExtractor.descriptorSize(), featureExtractor.descriptorType());
    r = 0;
    for (unsigned int i = 0; i < n; i++)
    {
        if (descriptorsPerTexture[i].rows == 0) continue;

        descriptors(cv::Rect(0, r, descriptors.cols, descriptorsPerTexture[i].rows)) = descriptorsPerTexture[i];
        r += descriptorsPerTexture[i].rows;
    }
}
