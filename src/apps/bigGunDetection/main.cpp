#include <opencv2/opencv.hpp>

#include "vtsense/MotionDetection/backgroundsubtractordetector.h"
#include "vtsense/ObjectDetection/hogpeopledetector.h"
#include "vtsense/MachineLearning/probabilisticmodel.h"
#include "vtsense/MachineLearning/svm.h"

int main(int, char *[])
{
    cv::Scalar red(0, 0, 255);
    cv::Scalar green(0, 255, 0);

    cv::VideoCapture video("/media/data/videoteror/videa3/DSC_0010.MOV");
    video.set(CV_CAP_PROP_POS_MSEC, 6 * 1000.0);

    VideoTeror::MachineLearning::ProbabilisticModel siluetteModel;
    cv::FileStorage pmStorage("/home/stepo/git/videoTeror/data/pca-siluette", cv::FileStorage::READ);
    siluetteModel.deserialize(pmStorage);

    VideoTeror::MachineLearning::SVM svm;
    cv::FileStorage svmStorage("/home/stepo/git/videoTeror/data/svm-biggun", cv::FileStorage::READ);
    svm.deserialize(svmStorage);

    cv::namedWindow("frame");

    VideoTeror::MotionDetection::BackgroundSubtractorDetector bgDetect(2, 4);
    VideoTeror::ObjectDetection::HogPeopleDetector peopleDetect;

    VideoTeror::BGRImage bgrFrame, guiFrame;
    VideoTeror::GrayscaleImage gsFrame;

    while(video.read(bgrFrame) && (char)cv::waitKey(1) != 27)
    {
        cv::cvtColor(bgrFrame, gsFrame, cv::COLOR_BGR2GRAY);
        bgrFrame.copyTo(guiFrame);

        VideoTeror::GrayscaleImage fgBitmap = bgDetect.detect(gsFrame);
        VideoTeror::ObjectDetection::ObjectDetector::DetectionResult::vector people = peopleDetect.detect(bgrFrame);

        for (const VideoTeror::ObjectDetection::ObjectDetector::DetectionResult &dr : people)
        {
            if (dr.score <= 1) continue;
            const cv::Rect &roi = dr.toProperRegion(bgrFrame.cols, bgrFrame.rows);
            if (roi.x < 0 || roi.y < 0 || roi.x + roi.width >= bgrFrame.cols || roi.y + roi.height >= bgrFrame.rows) continue;

            VideoTeror::MatF params = siluetteModel.getParams(fgBitmap(roi));
            float score = svm.classify(params);

            cv::Scalar color = (score > 0) ? green : red;
            cv::rectangle(guiFrame, roi, color);
            cv::putText(guiFrame, std::to_string(score),
                        roi.tl() + cv::Point(0, 15),
                        CV_FONT_HERSHEY_SIMPLEX, 0.5, color);
        }

        cv::imshow("frame", guiFrame);
    }
    cv::destroyAllWindows();
}
