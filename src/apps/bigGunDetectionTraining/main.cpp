#include <opencv2/opencv.hpp>

#include "vtsense/MotionDetection/backgroundsubtractordetector.h"
#include "vtsense/ObjectDetection/hogpeopledetector.h"
#include "vtsense/Tracking/pyrlktracker.h"
#include "vtsense/Helpers/helpers.h"
#include "vtsense/MachineLearning/probabilisticmodel.h"
#include "vtsense/MachineLearning/svm.h"

struct SiluetteExtractionData
{
    cv::VideoCapture video;
    std::vector<VideoTeror::GrayscaleImage> trainData;
    std::vector<bool> trainDataFlags;
    VideoTeror::ObjectDetection::ObjectDetector::DetectionResult::vector lastDetect;

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
            std::cout << "readFrame() init" << std::endl;
            prevBgrFrame = bgrFrame.clone();
        }
        return result;
    }

    int getFlag(int detectionIndex, const cv::Size &frameSize)
    {
        for (unsigned int i = 0; i < pointsToTrack.size(); i++)
        {
            pointsMissCounter[i]++;
            if (lastDetect[detectionIndex].toPixelRegion(frameSize).contains(pointsToTrack[i]))
            {
                pointsMissCounter[i] = 0;
                return pointsFlag[i];
            }
        }
        return -1;
    }

    void cleanPoints()
    {
        std::vector<unsigned int> toDelete;
        for (unsigned int i = 0; i < pointsToTrack.size(); i++)
        {
            if (pointsMissCounter[i] >= 20)
            {
                toDelete.push_back(i);
            }
        }

        std::reverse(toDelete.begin(), toDelete.end());
        for (unsigned int i : toDelete)
        {
            pointsToTrack.erase(pointsToTrack.begin() + i);
            pointsFlag.erase(pointsFlag.begin() + i);
            pointsMissCounter.erase(pointsMissCounter.begin() + i);
        }
    }
};

void siluetteExtraction_onMouse(int event, int x, int y, int flags, void* userdata)
{
    if (event != cv::EVENT_LBUTTONUP && event != cv::EVENT_RBUTTONUP) return;
    bool positive = (event == cv::EVENT_LBUTTONUP);
    SiluetteExtractionData &data = *(SiluetteExtractionData*)userdata;

    std::cout << "onMouse" << std::endl;

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

        cv::Size frameSize(data.bgrFrame.cols, data.bgrFrame.rows);
        for (unsigned int i = 0; i < data.lastDetect.size(); i++)
        {
            const VideoTeror::ObjectDetection::ObjectDetector::DetectionResult &dr = data.lastDetect[i];
            if (dr.score <= 1) continue;

            int flag = data.getFlag(i, frameSize);
            cv::Scalar color;
            if (flag == 1) color = green;
            else if (flag == 0) color = red;
            else color = white;
            cv::rectangle(guiFrame, dr.toPixelRegion(frameSize), color);
            cv::putText(guiFrame, std::to_string(dr.score),
                        dr.toPixelRegion(frameSize).tl() + cv::Point(0, 15),
                        CV_FONT_HERSHEY_SIMPLEX, 0.5, color);

            if (flag == 0 || flag == 1)
            {
                cv::Rect r = VideoTeror::Helpers::Helpers::crop(dr.toPixelRegion(frameSize), fgBitmap.cols, fgBitmap.rows);
                cv::Mat m;
                cv::resize(fgBitmap(r), m, cv::Size(32, 64));
                data.trainData.push_back(m);
                data.trainDataFlags.push_back(flag);
            }
        }
        for(unsigned int i = 0; i < data.pointsToTrack.size(); i++)
        {
            cv::circle(guiFrame, data.pointsToTrack[i], 10, data.pointsFlag[i] ? green : red, 3);
        }

        std::cout << "trainData size: " << data.trainData.size()
                  << " positives: " << std::count(data.trainDataFlags.begin(), data.trainDataFlags.end(), true)
                  << " negatives: " << std::count(data.trainDataFlags.begin(), data.trainDataFlags.end(), false) << std::endl;

        cv::imshow("frame", guiFrame);
    }
    cv::destroyAllWindows();

    /*std::cout << "Calculating model..." << std::endl;
    VideoTeror::MachineLearning::ProbabilisticModel model;
    model.learn(data.trainData);
    std::cout << "...done" << std::endl;

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
    svm.serialize(svmStorage);*/

    return 0;
}
