#include <QInputDialog>
#include <QApplication>
#include <QDebug>

#include "vtsense/MotionDetection/simplemotiondetector.h"
#include "vtsense/MotionDetection/opticalflowmotiondetector.h"
#include "vtsense/MotionDetection/farnebackmotiondetector.h"
#include "vtsense/MotionDetection/gradientmotiondetector.h"
#include "vtsense/MotionDetection/backgroundsubtractordetector.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStringList detectors;
    detectors << "Simple" << "Gradient" << "Background subtraction" << "Optical flow" << "Farneback";
    bool ok;
    QString selection = QInputDialog::getItem(0, "Select motion detection algorithm", "Algorithm:", detectors, 0, false, &ok);
    if (!ok) return 0;

    cv::VideoCapture video("/media/data/videoteror/videa3/DSC_0010.MOV");
    video.set(CV_CAP_PROP_POS_MSEC, 15000);

    cv::Mat rgbInit;
    video >> rgbInit;
    VideoTeror::GrayscaleImage init;

    cv::cvtColor(rgbInit, init, cv::COLOR_BGR2GRAY);

    VideoTeror::MotionDetection::MotionDetector::Ptr detector;
    if (selection == detectors[0])
        detector = new VideoTeror::MotionDetection::SimpleMotionDetector(init, 10, 64);
    else if (selection == detectors[1])
        detector = new VideoTeror::MotionDetection::GradientMotionDetector(init, 5);
    else if (selection == detectors[2])
        detector = new VideoTeror::MotionDetection::BackgroundSubtractorDetector(2, 4);
    else if (selection == detectors[3])
        detector = new VideoTeror::MotionDetection::OpticalFlowMotionDetector(init, 15);
    else
        detector = new VideoTeror::MotionDetection::FarnebackMotionDetector(init, 5);

    cv::Scalar green(0, 255, 0);
    cv::Scalar gray(127, 127, 127);
    cv::Mat bgrFrame, gui;
    VideoTeror::GrayscaleImage frame;
    char key;
    while(video.read(bgrFrame) && (key = cv::waitKey(30)) != 27)
    {
        cv::cvtColor(bgrFrame, frame, cv::COLOR_BGR2GRAY);
        bgrFrame.copyTo(gui);
        VideoTeror::GrayscaleImage result = detector->detect(frame);

        auto rectangles = detector->getBoundingRectangles(result, 10000, 480*240);
        foreach (const auto &rect, rectangles)
        {
            cv::rectangle(gui, rect, green);
        }

        VideoTeror::GrayscaleImage masked = frame.mul(result/255);
        cv::imshow("masked", masked);
        cv::imshow("frame", gui);

        if (key == ' ' && (char)cv::waitKey() == 's')
        {
            qDebug() << "Writing images";
            cv::imwrite("foreground.png", detector->getForeground());
            cv::imwrite("gui.png", gui);
            cv::imwrite("result.png", result);
            cv::imwrite("masked.png", masked);
            cv::imwrite("frame.png", bgrFrame);
        }
    }

    return 0;
}
