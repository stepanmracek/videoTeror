#include <QApplication>
#include <QDir>
#include <QInputDialog>
#include <objectdetection/facedetection.h>
#include <QDebug>

#include "Helpers/helpers.h"
#include "ObjectDetection/hogpeopledetector.h"
#include "MotionDetection/backgroundsubtractordetector.h"

void onMouse(int event, int x, int y, int flags, void* userdata)
{

    if (!(flags & cv::EVENT_FLAG_LBUTTON) && event != cv::EVENT_LBUTTONDOWN) return;
    cv::Rect *rect = (cv::Rect*)userdata;

    if (event == cv::EVENT_LBUTTONDOWN)
    {
        rect->x = x;
        rect->y = y;
    }
    else if (x > rect->x && y > rect->y)
    {
        rect->width = (x - rect->x);
        rect->height = (y - rect->y);
    }
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

    cv::Mat rgbInit;
    video >> rgbInit;
    VideoTeror::GrayscaleImage init;
    cv::cvtColor(rgbInit, init, cv::COLOR_BGR2GRAY);
    cv::Rect roi;

    VideoTeror::MotionDetection::BackgroundSubtractorDetector motionDetector(2, 4);
    VideoTeror::ObjectDetection::HogPeopleDetector peopleDetector;
    /*Face::ObjectDetection::FaceDetection faceDetector("/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml",
                                                      "/usr/share/opencv/haarcascades/haarcascade_lefteye_2splits.xml",
                                                      "/usr/share/opencv/haarcascades/haarcascade_righteye_2splits.xml", 1);*/
    cv::CascadeClassifier faceDetector("/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml"); //("../../data/haar-face.xml");
    std::vector<cv::Rect> faces;

    cv::namedWindow("frame");
    cv::setMouseCallback("frame", onMouse, &roi);

    cv::Scalar white(255, 255, 255);
    cv::Scalar green(0, 255, 0);
    cv::Mat bgrFrame, gui;
    VideoTeror::GrayscaleImage frame;
    char key;
    int wait = 30;
    while(video.read(bgrFrame) && (key = cv::waitKey(wait)) != 27)
    {
        if (key == ' ') wait = 0;
        cv::cvtColor(bgrFrame, frame, cv::COLOR_BGR2GRAY);
        bgrFrame.copyTo(gui);

        //auto detectionResult = peopleDetector.detect(bgrFrame);
        //foreach (const cv::Rect &roi, detectionResult.objects)
        //    cv::rectangle(gui, roi, green);

        if (roi.area())
        {
            cv::rectangle(gui, roi, white);
            //auto faces = faceDetector.detect(frame(roi), false);
            faceDetector.detectMultiScale(frame(roi), faces);
            for (const cv::Rect &faceReg : faces)
            {
                cv::rectangle(gui, cv::Rect(roi.x + faceReg.x, roi.y + faceReg.y, faceReg.width, faceReg.height), green);
            }
        }
        else
        {
            cv::putText(gui, "Select ROI first", cv::Point(frame.cols/4, frame.rows/2), cv::FONT_HERSHEY_SIMPLEX, 1, green, 1, CV_AA);
        }

        cv::imshow("frame", gui);

        if (key == VideoTeror::Helpers::Helpers::rightKey)
        {
            double pos = video.get(CV_CAP_PROP_POS_MSEC) + 5000;
            video.set(CV_CAP_PROP_POS_MSEC, pos);
        }
    }

    return 0;
}
