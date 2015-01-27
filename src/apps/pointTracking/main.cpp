#include <QApplication>
#include <QInputDialog>
#include <QDir>

#include "vtsense/Tracking/pyrlktracker.h"
#include "vtsense/Helpers/helpers.h"
#include "vtsenseExtras/dircrawler.h"

int nearestIndex(VideoTeror::Points* points, const VideoTeror::Point &p)
{
    int minIndex = -1;
    float minDistance = FLT_MAX;

    for (int i = 0; i < points->size(); i++)
    {
        float d = VideoTeror::Helpers::Helpers::euclDist(p, points->at(i));
        if (d < minDistance)
        {
            minDistance = d;
            minIndex = i;
        }
    }

    return minIndex;
}

void remove(VideoTeror::Points* points, int index)
{
    VideoTeror::Points copy = *points;
    int n = points->size();
    points->clear();

    for (int i = 0; i < n; i++)
    {
        if (i != index)
        {
            points->push_back(copy.at(i));
        }
    }
}

void onMouse(int event, int x, int y, int flags, void* userdata)
{
    if (event != cv::EVENT_LBUTTONUP && event != cv::EVENT_RBUTTONUP) return;
    VideoTeror::Points *points = (VideoTeror::Points*)userdata;

    VideoTeror::Point p(x, y);
    if (event == cv::EVENT_LBUTTONUP)
    {
        points->push_back(p);
    }
    else
    {
        int minIndex = nearestIndex(points, p);
        if (minIndex != -1)
            remove(points, minIndex);
    }
}

int main(int argc, char *argv[])
{
    QStringList filters; filters << "*.mov";
    QDir dir("/media/data/videoteror/");
    QStringList videos = VideoTeror::Extras::DirCrawler::recursiveSearch(dir, filters);

    QApplication app(argc, argv);
    bool ok;
    QString selection = QInputDialog::getItem(NULL, "Select video", "video:", videos, 0, false, &ok);
    if (!ok) return 0;

    VideoTeror::Points points;
    VideoTeror::Tracking::PyrLKTracker tracker;
    cv::namedWindow("video");
    cv::setMouseCallback("video", onMouse, &points);
    cv::VideoCapture video(selection.toStdString());

    cv::Scalar green(0, 255, 0);
    VideoTeror::BGRImage frame, prev, gui;
    video.read(prev);
    char key;
    while((key = cv::waitKey(25)) != 27 && video.read(frame))
    {
        frame.copyTo(gui);
        if (key == ' ') cv::waitKey();

        if (!points.empty()) points = tracker.track(prev, frame, points);
        for (VideoTeror::Point p : points)
        {
            cv::circle(gui, p, 5, green);
        }

        cv::imshow("video", gui);
        frame.copyTo(prev);
    }

    return 0;
}
