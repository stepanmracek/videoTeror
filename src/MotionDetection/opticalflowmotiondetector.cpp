#include "opticalflowmotiondetector.h"

VideoTeror::MotionDetection::OpticalFlowMotionDetector::OpticalFlowMotionDetector(const GrayscaleImage &initialFrame, int blockSize/*, double threshold*/) :
    prevFrame(initialFrame), blockSize(blockSize)//, threshold(threshold)
{
    //assert((threshold > 0.0) && (threshold < 1.0));
}

VideoTeror::GrayscaleImage VideoTeror::MotionDetection::OpticalFlowMotionDetector::detect(const GrayscaleImage &curFrame)
{
    GrayscaleImage result = Bitmap::zeros(curFrame.rows/blockSize, curFrame.cols/blockSize);

    std::vector<cv::Point2f> prevPoints;
    for (int y = blockSize/2 - 1; y < curFrame.rows; y += blockSize)
    {
        for (int x = blockSize/2 - 1; x < curFrame.cols; x += blockSize)
        {
            prevPoints.push_back(cv::Point2f(x, y));
        }
    }

    std::vector<cv::Point2f> nextPoints;
    std::vector<unsigned char> status;
    std::vector<float> err;
    cv::calcOpticalFlowPyrLK(prevFrame, curFrame, prevPoints, nextPoints, status, err);

    int i = 0;
    GrayscaleImage test;
    curFrame.copyTo(test);
    for (int y = 0; y < curFrame.rows; y += blockSize)
    {
        for (int x = 0; x < curFrame.cols; x += blockSize)
        {
            if (status[i] == 1)
            {
                double size = sqrt(pow(prevPoints[i].x-nextPoints[i].x,2) + pow(prevPoints[i].y-nextPoints[i].y,2));
                result(y/blockSize, x/blockSize) = 255*size;
                cv::line(test, prevPoints[i], nextPoints[i], 255);
            }

            i++;
        }
    }
    cv::imshow("flow", test);

    curFrame.copyTo(prevFrame);
    morphClosure(result);
    cv::resize(result, result, cv::Size(curFrame.cols, curFrame.rows), 0, 0, cv::INTER_NEAREST);
    return result;
}
