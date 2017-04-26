#include <QApplication>
#include <QInputDialog>
#include <QDir>
#include <QDebug>
#include <faceCommon/objectdetection/facedetection.h>
#include <faceCommon/linalg/imagefilterfactory.h>
#include <faceCommon/linalg/matrixconverter.h>
#include "vtsenseExtras/dircrawler.h"
#include "vtsense/Helpers/helpers.h"

void drawDetectionResult(const Face::ObjectDetection::FaceDetection::DetectionResult &result,
                         cv::Mat &img, const cv::Scalar &color)
{
    for (int i = 0; i < result.faceRegions.size(); i++)
    {
        cv::rectangle(img, result.faceRegions[i], color);
        cv::rectangle(img, result.leftEyes[i], color);
        cv::rectangle(img, result.rightEyes[i], color);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Face::ObjectDetection::FaceDetection detector("/usr/share/OpenCV/haarcascades/haarcascade_frontalface_default.xml",
                                                  "/usr/share/OpenCV/haarcascades/haarcascade_lefteye_2splits.xml",
                                                  "/usr/share/OpenCV/haarcascades/haarcascade_righteye_2splits.xml");

    QStringList videoDeviceFilter; videoDeviceFilter << "video*";
    QStringList videoDevices = VideoTeror::Extras::DirCrawler::recursiveSearch(QDir("/dev/"), videoDeviceFilter);

    bool ok;
    QString deviceName = QInputDialog::getItem(NULL, "Select video device", "Detected video devices:", videoDevices, 0, false, &ok);
    if (!ok) return 0;

    int deviceNumber = deviceName.split(QDir::separator()).last().split("video").last().toInt();
    qDebug() << deviceNumber;

    cv::VideoCapture videoCapture(deviceNumber);
    char key;
    VideoTeror::BGRImage frame;
    VideoTeror::GrayscaleImage gsFrame;
    VideoTeror::GrayscaleImage normFaces(150, 600);
    while ((key = cv::waitKey(1)) != 27 && videoCapture.read(frame))
    {
        cv::flip(frame, frame, 1);
        cv::cvtColor(frame, gsFrame, CV_BGR2GRAY);

        auto result = detector.detect(gsFrame, true);
        drawDetectionResult(result, frame, cv::Scalar(0, 255, 0));

        cv::imshow("frame", frame);

        normFaces = VideoTeror::GrayscaleImage::zeros(150, 600);
        for (int i = 0; i < result.normalizedFaceImages.size(); i++)
        {
            result.normalizedFaceImages[i].copyTo(normFaces(cv::Rect(i*150, 0, 150, 150)));
        }

        cv::imshow("faces", normFaces);

        if (key == ' ' && (char)cv::waitKey() == 's')
        {
            qDebug() << "Writing images";
            cv::imwrite("frame.png", frame);
            cv::imwrite("faces.png", normFaces);

            if (result.normalizedFaceImages.size() > 0)
            {
                std::vector<Face::LinAlg::ImageFilter::Ptr> filters = Face::LinAlg::ImageFilterFactory::create("dog-5-3;contrast;gaborAbs-3-4", ";");

                for (int max = 1; max <= filters.size(); max++)
                {
                    Matrix img = Face::LinAlg::MatrixConverter::grayscaleImageToDoubleMatrix(result.normalizedFaceImages[0]);
                    for (int i = 0; i < max; i++)
                    {
                        img = filters[i]->process(img);
                    }
                    double mn, mx; cv::minMaxIdx(img, &mn, &mx);
                    cv::imwrite((QString::number(max) + ".png").toStdString(), (img-mn)/(mx-mn)*255);
                }
            }
        }
    }
}


