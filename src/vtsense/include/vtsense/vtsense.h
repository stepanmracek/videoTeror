#ifndef VT_COMMON_H
#define VT_COMMON_H

#include <opencv2/opencv.hpp>

namespace VideoTeror
{

typedef cv::Mat_<float> MatF;
typedef cv::Mat_<double> MatD;
typedef cv::Mat_<cv::Vec3b> BGRImage;
typedef cv::Mat_<unsigned char> GrayscaleImage;
//typedef cv::Mat_<bool> Bitmap;

typedef cv::Point2f Point;
typedef std::vector<Point> Points;

}

#endif // VT_COMMON_H
