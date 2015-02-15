#include "vtsense/ObjectDetection/hogpeopledetector.h"
#include "vtsense/Tracking/objecttracker.h"

struct Result
{
    Result()
    {
        personCount = 0;
    }

    int personCount;
    std::map<int, std::vector<cv::Rect>> persons;
};

int main(int argc, char *argv[])
{
    /*cv::Point2f p1(0.123, 0.456);
    cv::Point2d p2 = p1;
    cv::Point2f p3 = p2;
    cv::Point2d p4 = p3;
    std::cout << p1.x << " " << p2.x << " " << p3.x << " " << p4.x << std::endl;
    std::cout << p1.y << " " << p2.y << " " << p3.y << " " << p4.y << std::endl;*/

    cv::Scalar green(0, 255, 0);

    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " /path/to/video.file" << std::endl;
        return 0;
    }

    cv::VideoCapture video(argv[1]);
    VideoTeror::ObjectDetection::HogPeopleDetector detector(0.5, 3.0);
    VideoTeror::Tracking::ObjectTracker tracker(detector);
    VideoTeror::Tracking::ObjectTracker::Result result;

    int index = 260;
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

    VideoTeror::GrayscaleImage trajectories = VideoTeror::GrayscaleImage::ones(s)*255;
    for (const std::pair<int, VideoTeror::Tracking::Trajectory> &t : result.trajectories)
    {
        for (int i = 1; i < t.second.points.size(); i++)
        {
            cv::Point prev(t.second.points[i-1].x * s.width, t.second.points[i-1].y * s.height);
            cv::Point next(t.second.points[i].x * s.width, t.second.points[i].y * s.height);
            cv::line(trajectories, prev, next, 0);
        }
    }
    cv::imshow("trajectories", trajectories);
    cv::waitKey(0);

    return 0;
}
