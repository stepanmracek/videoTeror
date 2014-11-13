#include <QApplication>
#include <QInputDialog>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include "ObjectDetection/hogpeopledetector.h"
#include "Tracking/pyrlktracker.h"
#include "Helpers/helpers.h"

void cleanUpPoints(VideoTeror::Points &points, QVector<int> &missCounter, const QVector<int> &toRemove)
{
    VideoTeror::Points newPoints;
    QVector<int> newMissCounter;

    for (int i = 0; i < points.size(); i++)
    {
        if (!toRemove.contains(i))
        {
            newPoints.push_back(points[i]);
            newMissCounter.push_back(missCounter[i]);
        }
    }

    points = newPoints;
    missCounter = newMissCounter;
}

int main(int argc, char *argv[])
{
    QStringList filters; filters << "*.mov";
    QDir dir("/media/data/videoteror/");
    QStringList videos = VideoTeror::Helpers::Helpers::recursiveSearch(dir, filters);

    QApplication app(argc, argv);
    bool ok;
    QString selection = QInputDialog::getItem(NULL, "Select video", "video:", videos, 0, false, &ok);
    if (!ok) return 0;

    cv::VideoCapture video(selection.toStdString());

    VideoTeror::Points points;
    QVector<int> missCounter;
    int personCounter = 0;
    int forgetThreshold = 120;

    VideoTeror::ObjectDetection::HogPeopleDetector detector(0.5, 3.0);
    VideoTeror::Tracking::PyrLKTracker tracker;

    cv::Scalar green(0, 255, 0);
    cv::Scalar red(0, 0, 255);
    VideoTeror::BGRImage frame, prev, gui;
    video.read(prev);
    char key;
    while((key = cv::waitKey(1)) != 27 && video.read(frame))
    {
        frame.copyTo(gui);
        if (key == ' ') cv::waitKey();
        VideoTeror::ObjectDetection::ObjectDetector::DetectionResult people = detector.detect(frame);

        for (int i = 0; i < people.objects.count(); i++)
        {
            cv::Rect rect = people.objects[i];
            cv::putText(gui, std::to_string(people.scores[i]), rect.tl() + cv::Point(0, -10), cv::FONT_HERSHEY_SIMPLEX, 0.5, green, 1, CV_AA);
            cv::rectangle(gui, rect, green);

            // is there some point already within the rectangle?
            bool hit = false;
            for (int j = 0; j < points.size(); j++)
            {
                if (rect.contains(points[j])) hit = true;
            }

            if (!hit)
            {
                personCounter++;
                VideoTeror::Point p(rect.x + rect.width*0.5, rect.y + rect.height*0.33);
                points.push_back(p);
                missCounter.push_back(0);
                qDebug() << "new point" << p.x << p.y;
                cv::rectangle(gui, rect, red, 3);
                cv::circle(gui, p, 10, red, 3);
                cv::imshow("video", gui);
                if ((char) cv::waitKey() == 's')
                {
                    cv::imwrite("shot.png", gui);
                }
            }
        }

        // is some point outside of rectangle for long time?
        QVector<int> toRemove;
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
            if (missCounter[pIndex] >= forgetThreshold) toRemove << pIndex;
        }
        cleanUpPoints(points, missCounter, toRemove);

        if (!points.empty()) points = tracker.track(prev, frame, points);
        for (VideoTeror::Point p : points)
        {
            cv::circle(gui, p, 5, green);
        }


        std::stringstream ss;
        ss << "detected persons: " << personCounter;
        cv::putText(gui, ss.str(), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, green, 1, CV_AA);

        cv::imshow("video", gui);
        frame.copyTo(prev);
    }

    return 0;
}
