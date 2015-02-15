#include "vtsense/ObjectDetection/hogpeopledetector.h"
#include "vtsense/Tracking/objecttracker.h"

int main(int argc, char *argv[])
{
    cv::Scalar green(0, 255, 0);

    if (argc < 2)
    {
        std::cout << "usage: " << argv[0] << " /path/to/video.file" << std::endl;
        return 0;
    }

    cv::VideoCapture video(argv[1]);
    VideoTeror::ObjectDetection::HogPeopleDetector detector(0.5, 3.0);
    VideoTeror::Tracking::ObjectTracker tracker(detector);
    VideoTeror::Tracking::ObjectTracker::Result result;

    int index = 0;
    video.set(CV_CAP_PROP_POS_FRAMES, index);

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
            cv::putText(gui, std::to_string(o.id) + ": " + std::to_string(o.score),
                        o.toPixelRegion(s).tl() + cv::Point(0, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, green, 1, CV_AA);
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
