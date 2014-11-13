#include <QVector>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <opencv2/opencv.hpp>

#include "MotionDetection/backgroundsubtractordetector.h"
#include "ObjectDetection/hogpeopledetector.h"
#include "Tracking/pyrlktracker.h"
#include "Helpers/helpers.h"
#include "MachineLearning/probabilisticmodel.h"
#include "MachineLearning/svm.h"

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
            if (pointsMissCounter[i] >= 20)
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

int main(int, char *[])
{
    cv::Scalar white(255, 255, 255);
    cv::Scalar red(0, 0, 255);
    cv::Scalar green(0, 255, 0);

    SiluetteExtractionData data("/media/data/videoteror/videa3/DSC_0010.MOV", 6);
    cv::namedWindow("frame");
    cv::setMouseCallback("frame", siluetteExtraction_onMouse, &data);

    VideoTeror::MotionDetection::BackgroundSubtractorDetector bgDetect(2, 4);
    VideoTeror::ObjectDetection::HogPeopleDetector peopleDetect;

    VideoTeror::BGRImage guiFrame;
    while(data.readFrame() && (char)cv::waitKey(1) != 27)
    {
        cv::cvtColor(data.bgrFrame, data.gsFrame, cv::COLOR_BGR2GRAY);
        VideoTeror::GrayscaleImage fgBitmap = bgDetect.detect(data.gsFrame);
        data.lastDetect = peopleDetect.detect(data.bgrFrame);
        data.bgrFrame.copyTo(guiFrame);

        if (data.pointsToTrack.size() > 0)
            data.pointsToTrack = data.tracker.track(data.prevBgrFrame, data.bgrFrame, data.pointsToTrack);
        data.cleanPoints();

        for (int i = 0; i < data.lastDetect.objects.size(); i++)
        {
            if (data.lastDetect.scores[i] <= 1) continue;

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

        cv::imshow("frame", guiFrame);
    }
    cv::destroyAllWindows();

    qDebug() << "Calculating model...";
    VideoTeror::MachineLearning::ProbabilisticModel model;
    model.learn(data.trainData);
    qDebug() << "...done";

    cv::FileStorage pcaStorage("pca", cv::FileStorage::WRITE);
    model.serialize(pcaStorage);
    model.show();

    QFile posFile("positives");
    posFile.open(QFile::WriteOnly);
    QTextStream posStream(&posFile);

    QFile negFile("negatives");
    negFile.open(QFile::WriteOnly);
    QTextStream negStream(&negFile);

    QFile modesPosFiles[10];
    QFile modesNegFiles[10];
    QTextStream modesPosStreams[10];
    QTextStream modesNegStreams[10];
    for (int mode = 0; mode < 10; mode++)
    {
        modesPosFiles[mode].setFileName("mode" + QString::number(mode) + "pos");
        modesPosFiles[mode].open(QFile::WriteOnly);
        modesPosStreams[mode].setDevice(&modesPosFiles[mode]);

        modesNegFiles[mode].setFileName("mode" + QString::number(mode) + "neg");
        modesNegFiles[mode].open(QFile::WriteOnly);
        modesNegStreams[mode].setDevice(&modesNegFiles[mode]);

    }

    QVector<VideoTeror::MatF> samples;
    qDebug() << "projecting samples...";
    for (int i = 0; i < data.trainData.count(); i++)
    {
        qDebug() << i;
        VideoTeror::MatF params = model.getParams(data.trainData[i]);
        qDebug() << "params" << params.rows << params.cols;
        QTextStream &s = data.trainDataFlags[i] ? posStream : negStream;
        s << params(0) << " " << params(1) << " " << params(2) << endl;
        samples << params;

        for (int mode = 0; mode < 10; mode++)
        {
            QTextStream &s = data.trainDataFlags[i] ? modesPosStreams[mode] : modesNegStreams[mode];
            s << params(mode) << endl;
        }
    }
    qDebug() << "...done";

    qDebug() << "training SVM...";
    VideoTeror::MachineLearning::SVM svm;
    svm.learn(samples, data.trainDataFlags);
    qDebug() << "...done";
    cv::FileStorage svmStorage("svm", cv::FileStorage::WRITE);
    svm.serialize(svmStorage);

    return 0;
}
