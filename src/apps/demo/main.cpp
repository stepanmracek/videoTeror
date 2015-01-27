#include "vtsense/ObjectDetection/hogpeopledetector.h"
#include "vtsense/Tracking/pyrlktracker.h"

void cleanUpPoints(VideoTeror::Points &points, std::vector<int> &missCounter, const std::vector<int> &toRemove)
{
    VideoTeror::Points newPoints;
    std::vector<int> newMissCounter;

    for (int i = 0; i < points.size(); i++)
    {
        if (std::find(toRemove.begin(), toRemove.end(), i) == toRemove.end())
        {
            newPoints.push_back(points[i]);
            newMissCounter.push_back(missCounter[i]);
        }
    }

    points = newPoints;
    missCounter = newMissCounter;
}

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
    Result result;

    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " /path/to/video.file" << std::endl;
        return 0;
    }

    cv::VideoCapture video(argv[1]);

    VideoTeror::Points points;
    std::vector<int> missCounter;
    int forgetThreshold = 120;

    VideoTeror::ObjectDetection::HogPeopleDetector detector(0.5, 3.0);
    VideoTeror::Tracking::PyrLKTracker tracker;

    cv::Scalar green(0, 255, 0);
    cv::Scalar red(0, 0, 255);
    VideoTeror::BGRImage frame, prev, gui;
    int frameIndex = 0;
    video.read(prev);
    char key;
    while((key = cv::waitKey(1)) != 27 && video.read(frame))
    {
        frame.copyTo(gui);
        VideoTeror::ObjectDetection::ObjectDetector::DetectionResult people = detector.detect(frame);

        for (unsigned int i = 0; i < people.objects.size(); i++)
        {
            cv::Rect rect = people.objects[i];
            cv::putText(gui, std::to_string(people.scores[i]), rect.tl() + cv::Point(0, -10), cv::FONT_HERSHEY_SIMPLEX, 0.5, green, 1, CV_AA);
            cv::rectangle(gui, rect, green);
            result.persons[frameIndex].push_back(rect);

            // is there some point already within the rectangle?
            bool hit = false;
            for (int j = 0; j < points.size(); j++)
            {
                if (rect.contains(points[j])) hit = true;
            }

            if (!hit)
            {
                result.personCount++;
                VideoTeror::Point p(rect.x + rect.width*0.5, rect.y + rect.height*0.33);
                points.push_back(p);
                missCounter.push_back(0);
                //qDebug() << "new point" << p.x << p.y;
                cv::rectangle(gui, rect, red, 3);
                cv::circle(gui, p, 10, red, 3);
                cv::imshow("video", gui);
                //cv::waitKey(1);
            }
        }

        // is some point outside of rectangle for long time?
        std::vector<int> toRemove;
        for (int pIndex = 0; pIndex < points.size(); pIndex++)
        {
            bool hit = false;
            for (const cv::Rect &rect : people.objects)
            {
                if (rect.contains(points[pIndex]))
                {
                    hit = true;
                    break;
                }
            }

            if (!hit) missCounter[pIndex]++;
            if (missCounter[pIndex] >= forgetThreshold) toRemove.push_back(pIndex);
        }
        cleanUpPoints(points, missCounter, toRemove);

        if (!points.empty()) points = tracker.track(prev, frame, points);
        for (VideoTeror::Point p : points)
        {
            cv::circle(gui, p, 5, green);
        }


        std::stringstream ss;
        ss << "detected persons: " << result.personCount;
        cv::putText(gui, ss.str(), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, green, 1, CV_AA);

        cv::imshow("video", gui);
        frame.copyTo(prev);
        frameIndex++;
    }

    for (const std::pair<int, std::vector<cv::Rect>> kvp : result.persons)
    {
        std::cout << "frame: " << kvp.first << std::endl;
        for (const cv::Rect &rect : result.persons[kvp.first])
        {
            std::cout << "  " << rect.x << " " << rect.y << " " << rect.width << " " << rect.height << std::endl;
        }
    }
    std::cout << "Detected persons: " << result.personCount << std::endl;

    return 0;
}
