#include "vtsense/MotionDetection/gradientmotiondetector.h"

VideoTeror::MotionDetection::GradientMotionDetector::GradientMotionDetector(GrayscaleImage &initialFrame, int blockSize, int historyLength) :
    blockSize(blockSize)
{
    this->historyLength = historyLength;
    GrayscaleImage initialResizedFrame;
    cv::resize(initialFrame, initialResizedFrame,
               cv::Size(initialFrame.cols / blockSize, initialFrame.rows / blockSize));

    cv::Canny(initialResizedFrame, initialResizedFrame, 196, 255);
    //cv::Sobel(initialResizedFrame, initialResizedFrame, -1, 1, 1);

    history.push_back(initialResizedFrame);
}

VideoTeror::GrayscaleImage VideoTeror::MotionDetection::GradientMotionDetector::detect(const GrayscaleImage &curFrame)
{
    GrayscaleImage resized;
    cv::resize(curFrame, resized, cv::Size(curFrame.cols / blockSize, curFrame.rows / blockSize));
    //cv::Canny(resized, resized, 196, 255);
    cv::Sobel(resized, resized, -1, 1, 1);

    cv::imshow("edges", resized);

    GrayscaleImage mean = getMeanHistoryImage();

    GrayscaleImage result(resized.rows, resized.cols);
    for (int r = 0; r < resized.rows; r++)
    {
        for (int c = 0; c < resized.cols; c++)
        {
            result(r,c) = 255 * (abs((int)(resized(r,c)) - (int)(mean(r,c))) > 32);
            //result(r, c) = abs((int)(resized(r,c)) - (int)(mean(r,c)));
        }
    }

    history.push_back(resized);
    if (history.size() > historyLength) history.pop_front();

    //cv::erode(result, result, cv::Mat());
    morphClosure(result, 4);

    cv::imshow("mean", mean);
    cv::resize(result, result, cv::Size(curFrame.cols, curFrame.rows), 0, 0, cv::INTER_NEAREST);

    return result;
}
