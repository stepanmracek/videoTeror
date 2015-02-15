#include <QApplication>
#include <QDir>
#include <QInputDialog>
#include <opencv2/opencv.hpp>

#include "vtsense/ObjectDetection/haardetector.h"
#include "vtsense/Tracking/objecttracker.h"
#include "vtsenseExtras/dircrawler.h"

int main(int argc, char *argv[])
{
    QStringList filters; filters << "*.mov";
    QDir dir("/media/data/videoteror/");
    QStringList videos = VideoTeror::Extras::DirCrawler::recursiveSearch(dir, filters);

    QApplication app(argc, argv);
    bool ok;
    QString selection = QInputDialog::getItem(NULL, "Select video", "video:", videos, 0, false, &ok);
    if (!ok) return 0;

    cv::VideoCapture video(selection.toStdString());

    cv::Scalar green(0, 255, 0);

    VideoTeror::ObjectDetection::HaarDetector detector("/home/stepo/git/videoTeror/data/haar-face.xml", 0.25,
                                                       cv::Size(10, 10), cv::Size(40, 40));
    VideoTeror::Tracking::ObjectTracker tracker(detector);
    VideoTeror::Tracking::ObjectTracker::Result result;

    int index = 0;
    VideoTeror::BGRImage frame, prev, gui;
    video.read(prev);
    cv::Size s(prev.cols, prev.rows);

    index++;
    while (video.read(frame) && cv::waitKey(1) < 0)
    {
        tracker.detectAndTrack(prev, frame, index, result);
        frame.copyTo(gui);

        for (const VideoTeror::ObjectDetection::ObjectDetector::DetectionResult &o : result.objectsPerFrame[index])
        {
            cv::rectangle(gui, o.toPixelRegion(s), green);
            cv::circle(gui, o.toPixelPoint(s), 3, green, 1, CV_AA);
            cv::putText(gui, std::to_string(o.id), o.toPixelRegion(s).tl() + cv::Point(0, 15),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, green, 1, CV_AA);
        }

        frame.copyTo(prev);

        cv::imshow("video", gui);
        index++;
    }

    VideoTeror::GrayscaleImage trajectories = result.drawTrajectories(s);
    cv::imshow("trajectories", trajectories);
    cv::waitKey(0);

    return 0;
}
