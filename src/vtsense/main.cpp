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
#include "Helpers/helpers.h"

#include "Linalg/probabilisticmodel.h"

#include <opencv2/nonfree/nonfree.hpp>

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
    //VideoTeror::MotionDetection::GradientMotionDetector detector(init, 5);
    VideoTeror::MotionDetection::BackgroundSubtractorDetector detector(4);
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

struct SiluetteExtractionData
{
    cv::VideoCapture video;
    QVector<VideoTeror::GrayscaleImage> trainData;
    QVector<bool> trainDataFlags;
    VideoTeror::ObjectDetection::ObjectDetector::DetectionResult lastDetect;

    cv::Mat prevBgrFrame;
    cv::Mat bgrFrame;
    VideoTeror::GrayscaleImage gsFrame;

    VideoTeror::Tracking::PyrLKTracker tracker;
    VideoTeror::Points pointsToTrack;
    std::vector<bool> pointsFlag;
    std::vector<int> pointsMissCounter;

    SiluetteExtractionData(const std::string &videoPath, double pos)
    {
        video.open(videoPath);
        video.set(CV_CAP_PROP_POS_MSEC, pos*1000.0);
    }

    bool readFrame()
    {
        prevBgrFrame = bgrFrame.clone();
        bool result = video.read(bgrFrame);
        if (prevBgrFrame.rows == 0)
        {
            qDebug() << "readFrame() init";
            prevBgrFrame = bgrFrame.clone();
        }
        return result;
    }

    int getFlag(int detectionIndex)
    {
        for (int i = 0; i < pointsToTrack.size(); i++)
        {
            pointsMissCounter[i]++;
            if (lastDetect.objects[detectionIndex].contains(pointsToTrack[i]))
            {
                pointsMissCounter[i] = 0;
                return pointsFlag[i];
            }
        }
        return -1;
    }

    void cleanPoints()
    {
        QVector<int> toDelete;
        for (int i = 0; i < pointsToTrack.size(); i++)
        {
            if (pointsMissCounter[i] >= 5)
            {
                toDelete << i;
            }
        }
        std::reverse(toDelete.begin(), toDelete.end());

        QVector<VideoTeror::Point> qPoints = QVector<VideoTeror::Point>::fromStdVector(pointsToTrack);
        QVector<bool> qFlags = QVector<bool>::fromStdVector(pointsFlag);
        QVector<int> qMisses = QVector<int>::fromStdVector(pointsMissCounter);
        foreach (int i, toDelete)
        {
            qPoints.removeAt(i);
            qFlags.removeAt(i);
            qMisses.removeAt(i);
        }

        pointsToTrack = qPoints.toStdVector();
        pointsFlag = qFlags.toStdVector();
        pointsMissCounter = qMisses.toStdVector();
    }
};

void siluetteExtraction_onMouse(int event, int x, int y, int flags, void* userdata)
{
    if (event != cv::EVENT_LBUTTONUP && event != cv::EVENT_RBUTTONUP) return;
    bool positive = (event == cv::EVENT_LBUTTONUP);
    SiluetteExtractionData &data = *(SiluetteExtractionData*)userdata;

    qDebug() << "onMouse";

    data.pointsToTrack.push_back(VideoTeror::Point(x, y));
    data.pointsFlag.push_back(positive);
    data.pointsMissCounter.push_back(0);
}

int mainSiluetteExtraction(int argc, char *argv[])
{
    cv::Scalar white(255, 255, 255);
    cv::Scalar red(0, 0, 255);
    cv::Scalar green(0, 255, 0);

    SiluetteExtractionData data("/media/data/videoteror/videa3/DSC_0010.MOV", 6);
    cv::namedWindow("frame");
    cv::setMouseCallback("frame", siluetteExtraction_onMouse, &data);

    VideoTeror::MotionDetection::BackgroundSubtractorDetector bgDetect(2);
    VideoTeror::ObjectDetection::HogPeopleDetector peopleDetect;

    VideoTeror::BGRImage guiFrame;
    while(data.readFrame() && (char)cv::waitKey(0) != 27)
    {
        cv::cvtColor(data.bgrFrame, data.gsFrame, cv::COLOR_BGR2GRAY);
        VideoTeror::Bitmap fgBitmap = bgDetect.detect(data.gsFrame);
        data.lastDetect = peopleDetect.detect(data.bgrFrame);
        data.bgrFrame.copyTo(guiFrame);

        if (data.pointsToTrack.size() > 0)
            data.pointsToTrack = data.tracker.track(data.prevBgrFrame, data.bgrFrame, data.pointsToTrack);
        data.cleanPoints();

        for (int i = 0; i < data.lastDetect.objects.size(); i++)
        {
            if (data.lastDetect.scores[i] <= 1) break;

            int flag = data.getFlag(i);
            cv::Scalar color;
            if (flag == 1) color = green;
            else if (flag == 0) color = red;
            else color = white;
            cv::rectangle(guiFrame, data.lastDetect.objects[i], color);
            cv::putText(guiFrame, std::to_string(data.lastDetect.scores[i]),
                        data.lastDetect.objects[i].tl() + cv::Point(0, 15),
                        CV_FONT_HERSHEY_SIMPLEX, 0.5, color);

            if (flag == 0 || flag == 1)
            {
                cv::Rect r = VideoTeror::Helpers::Helpers::crop(data.lastDetect.objects[i], fgBitmap.cols, fgBitmap.rows);
                cv::Mat m;
                cv::resize(fgBitmap(r), m, cv::Size(32, 64));
                data.trainData << m;
                data.trainDataFlags << flag;
            }
        }
        for(int i = 0; i < data.pointsToTrack.size(); i++)
        {
            cv::circle(guiFrame, data.pointsToTrack[i], 10, data.pointsFlag[i] ? green : red, 3);
        }

        qDebug() << "trainData size:" << data.trainData.size()
                 << "positives:" <<data.trainDataFlags.count(true)
                 << "negatives:" << data.trainDataFlags.count(false);
        //if (trainData.count() >= 50) break;

        //cv::waitKey(0);
        //cv::destroyAllWindows();

        //VideoTeror::GrayscaleImage masked = frame.mul(fgBitmap/255); // /255);
        //cv::imshow("masked", masked);
        cv::imshow("frame", guiFrame);
    }

    //VideoTeror::Linalg::ProbabilisticModel model;
    //model.learn(trainData);
    //model.show();

    return 0;
}

int mainAnotateTemplates(int argc, char *argv[])
{
    cv::VideoCapture video("/media/data/videoteror/videa3/DSC_0010.MOV");
    video.set(CV_CAP_PROP_POS_MSEC, 15000);
    QVector<VideoTeror::BGRImage> templates = VideoTeror::Helpers::Anotation::anotateRegions(video);
    VideoTeror::Helpers::Serialization::serialize("templates.yml", templates);
}

int mainObjectDetection(int argc, char *argv[])
{  
    QVector<VideoTeror::BGRImage> templates;
    VideoTeror::Helpers::Serialization::deserialize("../data/templates.yml", templates);

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
    while(video.read(frame))
    {
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
        cv::waitKey(1);
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
    /*QVector<VideoTeror::BGRImage> templates;
    VideoTeror::Helpers::Serialization::deserialize("templates.yml", templates);
    for (int i = 0; i < templates.size(); i++)
    {
        cv::imshow("template", templates[i]);
        cv::imwrite(("template-" + QString::number(i) + ".png").toStdString(), templates[i]);
        cv::waitKey();
    }*/

    //return mainAnotateTemplates(argc, argv);
    //return mainObjectDetection(argc, argv);
    //return mainMotion(argc, argv);
    return mainSiluetteExtraction(argc, argv);
    //return mainTracking(argc, argv);
}
