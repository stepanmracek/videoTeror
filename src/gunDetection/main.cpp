#include <QDebug>

#include "Helpers/serialization.h"
#include "Helpers/helpers.h"
#include "ObjectDetection/hogpeopledetector.h"
#include "ObjectDetection/templatematcher.h"
#include "Tracking/pyrlktracker.h"

int main(int argc, char *argv[])
{
    QVector<VideoTeror::BGRImage> templates;
    VideoTeror::Helpers::Serialization::deserialize("../../data/templates.yml", templates);

    VideoTeror::ObjectDetection::HogPeopleDetector detector(0.5, 3.0);
    VideoTeror::ObjectDetection::ObjectDetector::DetectionResult people;

    VideoTeror::ObjectDetection::TemplateMatcher matcher(templates);
    VideoTeror::Tracking::PyrLKTracker tracker;
    VideoTeror::Points points;

    cv::VideoCapture video("/media/data/videoteror/videa3/DSC_0010.MOV");
    video.set(CV_CAP_PROP_POS_MSEC, 5000);

    VideoTeror::BGRImage frame;
    VideoTeror::BGRImage prevFrame;
    video.read(prevFrame);

    qDebug() << "reading";
    char key;
    while((key = cv::waitKey(1)) != 27 && video.read(frame))
    {
        if (key == ' ') cv::waitKey();

        people = detector.detect(frame);
        int n = people.objects.count();
        qDebug() << "detected persons:" << n;

        QVector<VideoTeror::ObjectDetection::TemplateMatcher::Result> matchingResults(n);
        for (int i = 0; i < n; i++)
        {
            cv::Rect rect = VideoTeror::Helpers::Helpers::crop(people.objects[i], frame.cols, frame.rows);
            if ((rect.width <= 1) || (rect.height <= 1)) continue;

            matchingResults[i] = matcher.match(frame(rect));
        }

        for (int i = 0; i < n; i++)
        {
            cv::Rect &r = people.objects[i];
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

        foreach(const cv::Point2f &p, points)
        {
            cv::circle(frame, cv::Point(p.x, p.y), 3, cv::Scalar(0, 255, 255), 1, CV_AA);
        }

        for (int i = 0; i < n; i++)
        {
            cv::Rect &r = people.objects[i];
            int x = r.x + matchingResults[i].location.x;
            int y = r.y + matchingResults[i].location.y;
            cv::rectangle(frame, r, cv::Scalar(0,255,0));

            if (matchingResults[i].score > 0)
            {
                cv::putText(frame, QString::number(matchingResults[i].score).toStdString(), r.tl(), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,255,0), 1, CV_AA);
            }

            if (matchingResults[i].score > 0.9 && x > 20)
            {
                cv::circle(frame, cv::Point(x, y), 10, cv::Scalar(0, 0, 255), 1, CV_AA);
            }
        }

        cv::imshow("frame", frame);
    }

    return 0;
}
