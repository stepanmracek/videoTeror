#include <QApplication>
#include <QDir>
#include <QInputDialog>
#include <opencv2/opencv.hpp>

#include "vtsense/ObjectDetection/haardetector.h"
#include "vtsense/Tracking/objecttracker.h"
#include "vtsenseExtras/dircrawler.h"
#include "vtsense/ObjectDetection/dlibfacedetector.h"

void progressFunc(int percent)
{
    std::cout << "\r";
    for (int i = 0; i < percent; i+=2)
        std::cout << "#";
    int remain = 100 - percent;
    for (int i = 0; i < remain; i+=2)
        std::cout << "-";

    std::cout << " " << percent;
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

    cv::VideoCapture video(selection.toStdString());

    cv::Scalar green(0, 255, 0);

    //std::string cascadeFile = "/home/stepo/Stažené/cascadePUT.xml";
    //std::string cascadeFile = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt_tree.xml";
    //std::string cascadeFile = "/usr/share/opencv/lbpcascades/lbpcascade_frontalface.xml";
    //VideoTeror::ObjectDetection::HaarDetector detector(cascadeFile, 0.5, cv::Size(8,8), cv::Size(96,96));
    VideoTeror::ObjectDetection::DlibFaceDetector detector(1.0);

    VideoTeror::Tracking::ObjectTracker::Settings trackerSettings;
    //trackerSettings.forgetThreshold = 1;
    VideoTeror::Tracking::ObjectTracker tracker(detector, trackerSettings);
    VideoTeror::Tracking::ObjectTracker::Result result;

    //result = tracker.detectAndTrack(video, progressFunc);
    //std::cout << std::endl;

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
