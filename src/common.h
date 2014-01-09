#ifndef COMMON_H
#define COMMON_H

#include <opencv2/opencv.hpp>

namespace VideoTeror
{

typedef cv::Mat_<cv::Vec3b> BGRImage;
typedef cv::Mat_<unsigned char> GrayscaleImage;
typedef cv::Mat_<bool> Bitmap;

}

#endif // COMMON_H
