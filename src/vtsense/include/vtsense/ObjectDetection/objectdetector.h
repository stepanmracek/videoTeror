#ifndef VT_OBJECTDETECTOR_H
#define VT_OBJECTDETECTOR_H

#include <opencv2/opencv.hpp>

#include "vtsense/vtsense.h"

namespace VideoTeror
{
namespace ObjectDetection
{

class ObjectDetector
{
public:

    struct DetectionResult
    {
        int id;
        double score;
        cv::Point2d point;
        cv::Rect_<double> region;

        typedef std::vector<DetectionResult> vector;

        static const DetectionResult & findResult(const vector &objects, int id)
        {
            for (const auto &o : objects)
            {
                if (o.id == id) return o;
            }

            std::cerr << "object " << id << " not found" << std::endl;
        }

        cv::Point toPixelPoint(const cv::Size &frameSize) const
        {
            return cv::Point(point.x * frameSize.width, point.y * frameSize.height);
        }

        cv::Point toPixelPoint(int width, int height) const
        {
            return toPixelPoint(cv::Size(width, height));
        }

        cv::Rect toPixelRegion(const cv::Size &frameSize) const
        {
            return cv::Rect(region.x * frameSize.width, region.y * frameSize.height,
                            region.width * frameSize.width, region.height * frameSize.height);
        }

        cv::Rect toPixelRegion(int width, int height) const
        {
            return toPixelRegion(cv::Size(width, height));
        }
    };

    virtual DetectionResult::vector detect(const BGRImage &curFrame) = 0;
};

}
}
#endif // VT_OBJECTDETECTOR_H
