#include "vtsense/MotionDetection/backgroundsubtractordetector.h"

VideoTeror::MotionDetection::BackgroundSubtractorDetector::BackgroundSubtractorDetector(int morphClosureParam, int scaleParam) :
    morphClosureParam(morphClosureParam),
    scaleParam(scaleParam)
{

}

VideoTeror::GrayscaleImage VideoTeror::MotionDetection::BackgroundSubtractorDetector::getForeground()
{
    return foreground;
}

VideoTeror::GrayscaleImage VideoTeror::MotionDetection::BackgroundSubtractorDetector::detect(const GrayscaleImage &curFrame)
{
    GrayscaleImage frame;
    cv::resize(curFrame, frame, cv::Size(curFrame.cols/scaleParam, curFrame.rows/scaleParam));
    bs(frame, foreground);

    GrayscaleImage result = GrayscaleImage::zeros(frame.rows, frame.cols);
    for (int r = 0; r < frame.rows; r++)
    {
        for (int c = 0; c < frame.cols; c++)
        {
            if (foreground(r,c) > 0) result(r,c) = 255;
        }
    }

    morphClosure(result, morphClosureParam);
    cv::resize(result, result, cv::Size(curFrame.cols, curFrame.rows));

    //cv::Mat bg;
    //bs.getBackgroundImage(bg);
    //cv::imshow("foreground", foreground);
    return result.clone();
}
