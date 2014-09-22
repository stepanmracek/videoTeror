#include "motiondetector.h"

void VideoTeror::MotionDetection::MotionDetector::morphClosure(GrayscaleImage &bitmap, int parameter)
{
    if (parameter <= 0) return;

    //cv::imshow("before closure", bitmap);
    cv::dilate(bitmap, bitmap, cv::Mat(), cv::Point(-1, -1), parameter);
    cv::erode(bitmap, bitmap, cv::Mat(), cv::Point(-1, -1), parameter);
    //cv::dilate(bitmap, bitmap, cv::Mat(), cv::Point(-1, -1), parameter);
    //cv::imshow("after closure", bitmap);
}

VideoTeror::GrayscaleImage VideoTeror::MotionDetection::MotionDetector::getMeanHistoryImage()
{
    double norm = (1.0/history.size());
    const GrayscaleImage &f = history.front();
    GrayscaleImage result = GrayscaleImage::zeros(f.rows, f.cols);
    for (int r = 0; r < f.rows; r++)
    {
        for (int c = 0; c < f.cols; c++)
        {
            double val = 0;

            foreach (const VideoTeror::GrayscaleImage &i, history)
            {
                val += norm*i(r,c);
            }
            result(r,c) = val;
        }
    }

    return result;
}
