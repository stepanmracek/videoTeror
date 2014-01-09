#include "farnebackmotiondetector.h"

VideoTeror::MotionDetection::FarnebackMotionDetector::FarnebackMotionDetector(const GrayscaleImage &initialFrame, int blockSize/*, double threshold*/) :
    blockSize(blockSize)//, threshold(threshold)
{
    cv::resize(initialFrame, this->prevFrame,
               cv::Size(initialFrame.cols / blockSize, initialFrame.rows / blockSize));
}

VideoTeror::GrayscaleImage VideoTeror::MotionDetection::FarnebackMotionDetector::detect(const GrayscaleImage &curFrame)
{
    GrayscaleImage resized;
    cv::resize(curFrame, resized, cv::Size(prevFrame.cols, prevFrame.rows));

    cv::Mat_<cv::Vec2f> flow(resized.rows, resized.cols);

    cv::calcOpticalFlowFarneback(prevFrame, resized, flow, 0.75, 3, 10, 20, 5, 1.1, 0);

    GrayscaleImage test;
    curFrame.copyTo(test);

    GrayscaleImage result(resized.rows, resized.cols);
    for (int y = 0; y < resized.rows; y++)
    {
        for (int x = 0; x < resized.cols; x++)
        {
            cv::line(test, cv::Point(x*blockSize, y*blockSize), cv::Point(x*blockSize + flow(y, x)[0]*blockSize, y*blockSize + flow(y, x)[1]*blockSize), 255);
            result(y, x) = 255 * (sqrt(flow(y, x)[0]*flow(y, x)[0] + flow(y, x)[1]*flow(y, x)[1])); // > threshold);
        }
    }
    morphClosure(result);

    cv::imshow("test", test);

    resized.copyTo(prevFrame);
    cv::resize(result, result, cv::Size(curFrame.cols, curFrame.rows), 0, 0, cv::INTER_NEAREST);
    return result;
}
