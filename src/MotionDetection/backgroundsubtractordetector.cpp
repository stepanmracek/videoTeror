#include "backgroundsubtractordetector.h"

VideoTeror::MotionDetection::BackgroundSubtractorDetector::BackgroundSubtractorDetector()
{

}

VideoTeror::GrayscaleImage VideoTeror::MotionDetection::BackgroundSubtractorDetector::detect(const GrayscaleImage &curFrame)
{
    GrayscaleImage frame;
    cv::resize(curFrame, frame, cv::Size(curFrame.cols/4, curFrame.rows/4));
    bs(frame, foreground);

    GrayscaleImage result = Bitmap::zeros(frame.rows, frame.cols);
    for (int r = 0; r < frame.rows; r++)
    {
        for (int c = 0; c < frame.cols; c++)
        {
            if (foreground(r,c) > 0) result(r,c) = 255;
        }
    }

    morphClosure(result);
    cv::resize(result, result, cv::Size(curFrame.cols, curFrame.rows));
    return result;
}
