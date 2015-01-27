#include "vtsense/Helpers/helpers.h"

using namespace VideoTeror::Helpers;

VideoTeror::Points Helpers::merge(VideoTeror::Points &in, float mergeDistanceThreshold)
{
    VideoTeror::Points result;
    int n = in.size();
    if (n <= 1) return in;

    std::set<int> usedPoints;
    for (int i = 0; i < n; i++)
    {
        if (usedPoints.find(i) != usedPoints.end()) continue;
        //usedPoints << i;

        float sumx = in[i].x;
        float sumy = in[i].y;
        int counter = 1;
        for (int j = i + 1; j < n; j++)
        {
            if (usedPoints.find(j) != usedPoints.end()) continue;

            float d = euclDist(in[i], in[j]);;
            if (d < mergeDistanceThreshold)
            {
                usedPoints.insert(j);
                sumx += in[j].x;
                sumy += in[j].y;
                counter++;
            }
        }

        result.push_back(cv::Point2f(sumx/counter, sumy/counter));
    }

    //qDebug() << "merge" << in.size() << "->" << result.size();
    return result;
}

cv::Rect Helpers::crop(const cv::Rect &in, int maxX, int maxY)
{
    cv::Rect rect(in);
    if (rect.x < 0) rect.x = 0;
    if (rect.y < 0) rect.y = 0;
    if (rect.x + rect.width >= maxX) rect.width = maxX - rect.x;
    if (rect.y + rect.height >= maxY) rect.height = maxY - rect.y;
    return rect;
}

void Helpers::drawKeyPoints(cv::Mat &img, std::vector<cv::KeyPoint> &keypoints)
{
    for(const cv::KeyPoint keypoint : keypoints)
    {
        cv::circle(img, keypoint.pt, keypoint.size, cv::Scalar(255,255,255), 1, CV_AA);
    }
}

