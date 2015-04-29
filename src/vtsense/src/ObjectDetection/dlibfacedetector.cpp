#include "vtsense/ObjectDetection/dlibfacedetector.h"
#include "dlib/opencv/cv_image.h"

using namespace VideoTeror::ObjectDetection;

DlibFaceDetector::DlibFaceDetector(double scaleFactor) :
    scaleFactor(scaleFactor)
{
    detector = dlib::get_frontal_face_detector();
}

std::vector<ObjectDetector::DetectionResult> DlibFaceDetector::detect(const BGRImage &curFrame)
{
    std::vector<ObjectDetector::DetectionResult> result;

    cv::cvtColor(curFrame, frame, CV_BGR2GRAY);
    cv::resize(frame, resizedFrame, cv::Size(frame.cols*scaleFactor, frame.rows*scaleFactor));
    dlib::cv_image<unsigned char> dlibImage(resizedFrame);
    std::vector<dlib::rectangle> objects = detector(dlibImage);

    int id = 0;
    for (const dlib::rectangle &dlibRect : objects)
    {
        cv::Rect o(dlibRect.left(), dlibRect.top(), dlibRect.right() - dlibRect.left(), dlibRect.bottom() - dlibRect.top());
        ObjectDetector::DetectionResult r;

        r.id = id;
        r.point = cv::Point2d(((double)o.x + o.width/2) / resizedFrame.cols,
                              ((double)o.y + o.height/2) / resizedFrame.rows);
        r.region = cv::Rect_<double>(((double)o.x) / resizedFrame.cols,
                                     ((double)o.y) / resizedFrame.rows,
                                     ((double)o.width) / resizedFrame.cols,
                                     ((double)o.height) / resizedFrame.rows);
        r.score = 1.0;
        result.push_back(r);

        id++;
    }

    return result;
}
