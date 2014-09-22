#include "helpers.h"

#include <QSet>
#include <QDir>
#include <QStringList>

using namespace VideoTeror::Helpers;

VideoTeror::Points Helpers::merge(VideoTeror::Points &in, float mergeDistanceThreshold)
{
    VideoTeror::Points result;
    int n = in.size();
    if (n <= 1) return in;

    QSet<int> usedPoints;
    for (int i = 0; i < n; i++)
    {
        if (usedPoints.contains(i)) continue;
        //usedPoints << i;

        float sumx = in[i].x;
        float sumy = in[i].y;
        int counter = 1;
        for (int j = i + 1; j < n; j++)
        {
            if (usedPoints.contains(j)) continue;

            float d = euclDist(in[i], in[j]);;
            if (d < mergeDistanceThreshold)
            {
                usedPoints << j;
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
    foreach(const cv::KeyPoint keypoint, keypoints)
    {
        cv::circle(img, keypoint.pt, keypoint.size, cv::Scalar(255,255,255), 1, CV_AA);
    }
}

QStringList Helpers::recursiveSearch(const QDir &dir, const QStringList &filters)
{
    QStringList result;
    auto subdirs = dir.entryInfoList(QStringList(), QDir::Filters(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks),
                                     QDir::SortFlags(QDir::Name | QDir::IgnoreCase));
    foreach (const QFileInfo &subdir, subdirs)
    {
        result << recursiveSearch(QDir(subdir.absoluteFilePath()), filters);
    }

    auto files = dir.entryInfoList(filters, QDir::Files | QDir::System | QDir::NoDotAndDotDot | QDir::NoSymLinks,
                                   QDir::Name | QDir::IgnoreCase);
    foreach (const QFileInfo &file, files)
    {
        result << file.absoluteFilePath();
    }

    return result;
}
