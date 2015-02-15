#include "vtsense/Helpers/serialization.h"
#include "vtsense/Helpers/helpers.h"
#include "vtsense/ObjectDetection/hogpeopledetector.h"
#include "vtsense/ObjectDetection/templatematcher.h"
#include "vtsense/Tracking/pyrlktracker.h"

int main(int argc, char *argv[])
{
    std::vector<VideoTeror::BGRImage> templates;
    VideoTeror::Helpers::Serialization::deserialize("../../data/templates.yml", templates);

    VideoTeror::ObjectDetection::HogPeopleDetector detector(0.5, 3.0);
    VideoTeror::ObjectDetection::ObjectDetector::DetectionResult::vector people;

    VideoTeror::ObjectDetection::TemplateMatcher matcher(templates);
    VideoTeror::Tracking::PyrLKTracker tracker;
    VideoTeror::Points points;

    cv::VideoCapture video("/media/data/videoteror/videa3/DSC_0010.MOV");
    video.set(CV_CAP_PROP_POS_MSEC, 5000);

    VideoTeror::BGRImage frame;
    VideoTeror::BGRImage prevFrame;
    video.read(prevFrame);

    std::cout << "reading" << std::endl;
    char key;
    while((key = cv::waitKey(1)) != 27 && video.read(frame))
    {
        if (key == ' ') cv::waitKey();

        people = detector.detect(frame);
        unsigned int n = people.size();
        std::cout << "detected persons:" << std::endl;

        std::vector<VideoTeror::ObjectDetection::TemplateMatcher::Result> matchingResults(n);
        for (unsigned int i = 0; i < n; i++)
        {
            cv::Rect rect = VideoTeror::Helpers::Helpers::crop(people[i].toPixelRegion(frame.cols, frame.rows), frame.cols, frame.rows);
            if ((rect.width <= 1) || (rect.height <= 1)) continue;

            matchingResults[i] = matcher.match(frame(rect));
        }

        for (unsigned int i = 0; i < n; i++)
        {
            cv::Rect r = people[i].toPixelRegion(frame.cols, frame.rows);
            int x = r.x + matchingResults[i].location.x;
            int y = r.y + matchingResults[i].location.y;
            if (matchingResults[i].score > 0.9 && x > 20)
            {
                points.push_back(cv::Point2f(x, y));
            }
        }
        points = VideoTeror::Helpers::Helpers::merge(points, 20);

        if (points.size() > 0)
            points = tracker.track(prevFrame, frame, points);
        frame.copyTo(prevFrame);

        for(const cv::Point2f &p : points)
        {
            cv::circle(frame, cv::Point(p.x, p.y), 3, cv::Scalar(0, 255, 255), 2, CV_AA);
        }

        for (unsigned int i = 0; i < n; i++)
        {
            cv::Rect r = people[i].toPixelRegion(frame.cols, frame.rows);
            int x = r.x + matchingResults[i].location.x;
            int y = r.y + matchingResults[i].location.y;
            cv::rectangle(frame, r, cv::Scalar(0,255,0));

            if (matchingResults[i].score > 0)
            {
                cv::putText(frame, std::to_string(matchingResults[i].score), r.tl(), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,255,0), 1, CV_AA);
            }

            if (matchingResults[i].score > 0.9 && x > 20)
            {
                cv::circle(frame, cv::Point(x, y), 10, cv::Scalar(0, 0, 255), 3, CV_AA);
            }
        }

        cv::imshow("frame", frame);
    }

    return 0;
}
