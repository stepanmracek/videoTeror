#include <QApplication>
#include <QInputDialog>
#include <QDir>
#include <QDebug>
#include <objectdetection/facedetection.h>
#include <biometrics/multiextractor.h>

#include "Helpers/helpers.h"

void drawDetectionResult(const Face::ObjectDetection::FaceDetection::DetectionResult &result,
                         int index, cv::Mat &img, const cv::Scalar &color, const std::string &text = std::string())
{
    cv::rectangle(img, result.faceRegions[index], color);
    cv::rectangle(img, result.leftEyes[index], color);
    cv::rectangle(img, result.rightEyes[index], color);

    if (!text.empty())
        cv::putText(img, text, result.faceRegions[index].tl() + cv::Point(0, -5), CV_FONT_HERSHEY_SIMPLEX, 0.5, color, 1, CV_AA);
}

void drawDetectionResult(const Face::ObjectDetection::FaceDetection::DetectionResult &result,
                         cv::Mat &img, const cv::Scalar &color)
{
    for (int i = 0; i < result.faceRegions.size(); i++)
    {
        drawDetectionResult(result, i, img, color);
    }
}

int main(int argc, char *argv[])
{
    cv::Scalar green(0, 255, 0);
    cv::Scalar white(255, 255, 255);
    cv::Scalar red(0, 0, 255);
    QApplication app(argc, argv);

    QStringList videoDeviceFilter; videoDeviceFilter << "video*";
    QStringList videoDevices = VideoTeror::Helpers::Helpers::recursiveSearch(QDir("/dev/"), videoDeviceFilter);
    bool ok;
    QString deviceName = QInputDialog::getItem(NULL, "Select video device", "Detected video devices:", videoDevices, 0, false, &ok);
    if (!ok) return 0;
    int deviceNumber = deviceName.split(QDir::separator()).last().split("video").last().toInt();
    cv::VideoCapture videoCapture(deviceNumber);

    Face::ObjectDetection::FaceDetection detector("/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml",
                                                  "/usr/share/opencv/haarcascades/haarcascade_lefteye_2splits.xml",
                                                  "/usr/share/opencv/haarcascades/haarcascade_righteye_2splits.xml");

    Face::Biometrics::MultiExtractor extractor("/home/stepo/build/videoteror/data/2d-extractor-2");
    std::vector<Face::Biometrics::MultiTemplate> references;

    int threshold = 8;
    cv::namedWindow("frame");
    cv::createTrackbar("threshold", "frame", &threshold, 10);

    char key;
    VideoTeror::BGRImage frame;
    VideoTeror::GrayscaleImage gsFrame;
    while ((key = cv::waitKey(1)) != 27 && videoCapture.read(frame))
    {
        cv::flip(frame, frame, 1);
        cv::cvtColor(frame, gsFrame, CV_BGR2GRAY);
        auto result = detector.detect(gsFrame, true);

        drawDetectionResult(result, frame, white);

        if (key == ' ' && result.normalizedFaceImages.size() == 1)
        {
            references.push_back(extractor.extract(result.normalizedFaceImages[0], 1, 1));
        }
        else if (references.size() > 0)
        {
            for (int i = 0; i < result.normalizedFaceImages.size(); i++)
            {
                auto probe = extractor.extract(result.normalizedFaceImages[i], 1, 1);
                auto score = (-extractor.compare(references, probe, 3).distance) / 2 + 5;

                if (score < 0) score = 0;
                //if (score > 10) score = 10;

                drawDetectionResult(result, i, frame, score > threshold ? green : red, std::to_string(score));
            }
        }

        cv::imshow("frame", frame);
    }
}
