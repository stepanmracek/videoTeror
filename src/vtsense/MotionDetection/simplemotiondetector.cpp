#include "simplemotiondetector.h"

#include <QDebug>

VideoTeror::MotionDetection::SimpleMotionDetector::SimpleMotionDetector(GrayscaleImage &initialFrame, int blockSize, int threshold, int historyLength) :
    blockSize(blockSize),
    threshold(threshold)
{
    this->historyLength = historyLength;
    GrayscaleImage initialResizedFrame;
    cv::resize(initialFrame, initialResizedFrame,
               cv::Size(initialFrame.cols / blockSize, initialFrame.rows / blockSize));
    history.push_back(initialResizedFrame);
}

VideoTeror::GrayscaleImage VideoTeror::MotionDetection::SimpleMotionDetector::detect(const GrayscaleImage &curFrame)
{
    GrayscaleImage resized;
    cv::resize(curFrame, resized, cv::Size(curFrame.cols / blockSize, curFrame.rows / blockSize));

    GrayscaleImage mean = getMeanHistoryImage();

    GrayscaleImage result(resized.rows, resized.cols);
    for (int r = 0; r < resized.rows; r++)
    {
        for (int c = 0; c < resized.cols; c++)
        {
            result(r,c) = 255 * (abs((int)(resized(r,c)) - (int)(mean(r,c))) > threshold);
        }
    }

    history.push_back(resized);
    if (history.size() > historyLength) history.pop_front();

    morphClosure(result, 4);

    //cv::imshow("mean", mean);
    cv::resize(result, result, cv::Size(curFrame.cols, curFrame.rows), 0, 0, cv::INTER_NEAREST);

    return result;
}
