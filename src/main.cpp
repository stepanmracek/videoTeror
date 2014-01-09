#include <QDebug>
#include <QVector>

#include "MotionDetection/simplemotiondetector.h"
#include "MotionDetection/opticalflowmotiondetector.h"
#include "MotionDetection/farnebackmotiondetector.h"
#include "MotionDetection/gradientmotiondetector.h"
#include "MotionDetection/backgroundsubtractordetector.h"

#include "ObjectDetection/hogpeopledetector.h"
#include "ObjectDetection/templatematcher.h"

#include "Tracking/pyrlktracker.h"

#include "Helpers/anotation.h"
#include "Helpers/serialization.h"

#include <opencv2/nonfree/nonfree.hpp>

void drawKeyPoints(cv::Mat &img, std::vector<cv::KeyPoint> &keypoints)
{
    foreach(const cv::KeyPoint keypoint, keypoints)
    {
        cv::circle(img, keypoint.pt, keypoint.size, cv::Scalar(255,255,255), 1, CV_AA);
    }
}

int mainAnotate(int argc, char *argv[])
{
    cv::VideoCapture video("/media/data/videoteror/videa3/DSC_0001.MOV");
    QVector<VideoTeror::BGRImage> textures = VideoTeror::Helpers::Anotation::anotateRegions(video);
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    VideoTeror::Helpers::Anotation::extractFeatures(textures, keypoints, descriptors);
    VideoTeror::Helpers::Serialization::serialize("features.yml", keypoints, descriptors);
}

int mainMotion(int argc, char *argv[])
{
    cv::VideoCapture video("/media/data/videoteror/videa3/DSC_0010.MOV");
    video.set(CV_CAP_PROP_POS_MSEC, 15000);

    cv::Mat rgbInit;
    video >> rgbInit;
    VideoTeror::GrayscaleImage init;

    cv::cvtColor(rgbInit, init, cv::COLOR_BGR2GRAY);

    //VideoTeror::MotionDetection::SimpleMotionDetector detector(init, 10, 64);
    VideoTeror::MotionDetection::GradientMotionDetector detector(init, 5);
    //VideoTeror::MotionDetection::BackgroundSubtractorDetector detector;
    //VideoTeror::MotionDetection::OpticalFlowMotionDetector detector(init, 20);
    //VideoTeror::MotionDetection::FarnebackMotionDetector detector(init, 15, 0.5);

    cv::Mat rgbFrame;
    VideoTeror::GrayscaleImage frame;
    while(video.read(rgbFrame))
    {
        cv::cvtColor(rgbFrame, frame, cv::COLOR_BGR2GRAY);
        VideoTeror::Bitmap result = detector.detect(frame);

        VideoTeror::GrayscaleImage masked = frame.mul(result/255);
        cv::imshow("masked", masked);
        cv::imshow("frame", frame);
        cv::waitKey(0);
    }

    return 0;
}

int mainPeopleDetection(int argc, char *argv[])
{
    /*cv::VideoCapture cam(0);
    int fourCC = cam.get(CV_CAP_PROP_FOURCC);
    qDebug() << fourCC;
    cv::Mat camFrame;
    while(cam.read(camFrame))
    {
        cv::imshow("cam", camFrame);
        cv::waitKey(20);
    }
    return 0;*/

    cv::VideoCapture video("/media/data/videoteror/videa3/DSC_0010.MOV");
    video.set(CV_CAP_PROP_POS_MSEC, 15000);

    /*int fourCC = CV_FOURCC('D', 'I', 'V', 'X');
    double fps = video.get(CV_CAP_PROP_FPS);
    int w = video.get(CV_CAP_PROP_FRAME_WIDTH);
    int h = video.get(CV_CAP_PROP_FRAME_HEIGHT);
    cv::VideoWriter writer("out.avi", fourCC, fps, cv::Size(w, h), false);
    qDebug() << writer.isOpened();*/

    VideoTeror::ObjectDetection::HogPeopleDetector detector(0.5, 3.0);

    cv::Mat rgb;
    VideoTeror::BGRImage frame;
    while(video.read(rgb))
    {
        //cv::cvtColor(rgb, frame, cv::COLOR_BGR2GRAY);
        frame = rgb.clone();
        VideoTeror::ObjectDetection::ObjectDetector::DetectionResult people = detector.detect(frame);

        foreach(const cv::Rect &r, people.objects)
        {
            cv::rectangle(frame, r, 255);
        }
        std::stringstream ss;
        ss << "detected persons: " << people.objects.size();
        cv::putText(frame, ss.str(), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, 255);

        cv::imshow("frame", frame);
        cv::waitKey(1);
        //writer << frame;
    }

    return 0;
}

int mainTextureDetection(int argc, char *argv[])
{
    cv::VideoCapture video("/media/data/videoteror/videa3/DSC_0010.MOV");
    video.set(CV_CAP_PROP_POS_MSEC, 15000);
    QVector<VideoTeror::BGRImage> textures = VideoTeror::Helpers::Anotation::anotateRegions(video);
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    VideoTeror::Helpers::Anotation::extractFeatures(textures, keypoints, descriptors);

    /*std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    VideoTeror::Helpers::TextureAnotation::deserialize("features.yml", keypoints, descriptors);*/

    VideoTeror::ObjectDetection::HogPeopleDetector detector(1, 2.0);
    VideoTeror::ObjectDetection::ObjectDetector::DetectionResult people;

    video.set(CV_CAP_PROP_POS_MSEC, 15000);

    cv::BFMatcher matcher;
    //cv::FlannBasedMatcher matcher;
    std::vector<cv::Mat> trainDescriptors;
    trainDescriptors.push_back(descriptors);
    qDebug() << "adding trainDescriptors";
    matcher.add(trainDescriptors);
    qDebug() << "training";
    matcher.train();

    cv::ORB orb;
    VideoTeror::BGRImage frame;
    qDebug() << "reading";
    while(video.read(frame))
    {
        people = detector.detect(frame);
        qDebug() << "detected persons:" << people.objects.count();

        QVector<bool> found;

        foreach(const cv::Rect &r, people.objects)
        {
            cv::Rect rect(r);
            if (rect.x < 0) rect.x = 0;
            if (rect.y < 0) rect.y = 0;
            if (rect.x + rect.width >= frame.cols) rect.width = frame.cols - rect.x;
            if (rect.y + rect.height >= frame.rows) rect.height = frame.rows - rect.y;

            std::vector<cv::KeyPoint> probeKeyPoints;
            cv::Mat probeDescriptors;
            orb(frame(rect), cv::Mat(), probeKeyPoints, probeDescriptors);

            std::vector<cv::DMatch> matches;
            matcher.match(probeDescriptors, matches);

            float minDistance = 1e300;
            foreach (const cv::DMatch &m, matches)
            {
                if (m.distance < minDistance) minDistance = m.distance;
            }
            qDebug() << probeKeyPoints.size() << matches.size() << minDistance;
            found << (minDistance < 50);
        }

        for (int i = 0; i < found.count(); i++)
        {
            cv::rectangle(frame, people.objects[i], found[i] ? cv::Scalar(0,255,0) : cv::Scalar(0,0,255));
        }

        cv::imshow("frame", frame);
        cv::waitKey(1);
        //writer << frame;
    }

    return 0;
}

int mainAnotateTemplates(int argc, char *argv[])
{
    cv::VideoCapture video("/media/data/videoteror/videa3/DSC_0010.MOV");
    video.set(CV_CAP_PROP_POS_MSEC, 15000);
    QVector<VideoTeror::BGRImage> templates = VideoTeror::Helpers::Anotation::anotateRegions(video);
    VideoTeror::Helpers::Serialization::serialize("templates.yml", templates);
}

cv::Rect crop(const cv::Rect &in, int maxX, int maxY)
{
    cv::Rect rect(in);
    if (rect.x < 0) rect.x = 0;
    if (rect.y < 0) rect.y = 0;
    if (rect.x + rect.width >= maxX) rect.width = maxX - rect.x;
    if (rect.y + rect.height >= maxY) rect.height = maxY - rect.y;
    return rect;
}

inline float dist(cv::Point2f &p1, cv::Point2f &p2)
{
    return sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
}

VideoTeror::Tracking::Points merge(VideoTeror::Tracking::Points &in, float mergeDistanceThreshold)
{
    VideoTeror::Tracking::Points result;
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

            float d = dist(in[i], in[j]);;
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

    qDebug() << "merge" << in.size() << "->" << result.size();
    return result;
}

int mainObjectDetection(int argc, char *argv[])
{  
    QVector<VideoTeror::BGRImage> templates;
    VideoTeror::Helpers::Serialization::deserialize("templates.yml", templates);

    VideoTeror::ObjectDetection::HogPeopleDetector detector(0.5, 3.0);
    VideoTeror::ObjectDetection::ObjectDetector::DetectionResult people;

    VideoTeror::ObjectDetection::TemplateMatcher matcher(templates);
    VideoTeror::Tracking::PyrLKTracker tracker;
    VideoTeror::Tracking::Points points;

    cv::VideoCapture video("/media/data/videoteror/videa3/DSC_0010.MOV");
    video.set(CV_CAP_PROP_POS_MSEC, 5000);

    VideoTeror::BGRImage frame;
    VideoTeror::BGRImage prevFrame;
    video.read(prevFrame);

    qDebug() << "reading";
    while(video.read(frame))
    {
        people = detector.detect(frame);
        int n = people.objects.count();
        qDebug() << "detected persons:" << n;

        QVector<VideoTeror::ObjectDetection::TemplateMatcher::Result> matchingResults(n);
        for (int i = 0; i < n; i++)
        {
            cv::Rect rect = crop(people.objects[i], frame.cols, frame.rows);
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
        points = merge(points, 20);

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
        cv::waitKey(0);
    }

    return 0;
}

int mainTracking(int argc, char *argv[])
{
    cv::VideoCapture video("/media/data/videoteror/videa3/DSC_0010.MOV");
    video.set(CV_CAP_PROP_POS_MSEC, 15000);
    VideoTeror::BGRImage frame;
    video.read(frame);
    std::vector<cv::Point2f> points = VideoTeror::Helpers::Anotation::anotatePoints(frame);

    qDebug() << "tracking" << points.size() << "points of interest";
    VideoTeror::Tracking::PyrLKTracker tracker;

    VideoTeror::BGRImage motion = VideoTeror::BGRImage::zeros(frame.rows, frame.cols);

    VideoTeror::BGRImage prev;
    frame.copyTo(prev);
    while(video.read(frame))
    {
        std::vector<cv::Point2f> newPoints = tracker.track(prev, frame, points);;

        frame.copyTo(prev);
        for (int i = 0; i < points.size(); i++)
        {
            cv::Point2f &p = points[i];
            cv::Point2f &n = newPoints[i];

            cv::circle(frame, cv::Point(p.x, p.y), 3, cv::Scalar(0,255,255), 2, CV_AA);
            cv::line(motion, cv::Point(p.x, p.y), cv::Point(n.x, n.y), cv::Scalar(255,255,255), 1, CV_AA);
        }
        points = newPoints;

        cv::imshow("frame", frame);
        cv::imshow("motion", motion);
        cv::waitKey();
    }
    return 0;
}

int main(int argc, char *argv[])
{
    //return mainAnotateTemplates(argc, argv);
    return mainObjectDetection(argc, argv);
    //return mainMotion(argc, argv);
    //return mainTracking(argc, argv);
}
