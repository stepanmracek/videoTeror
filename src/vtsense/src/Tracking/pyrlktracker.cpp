#include "vtsense/Tracking/pyrlktracker.h"

using namespace VideoTeror::Tracking;

PyrLKTracker::PyrLKTracker()
{
}

VideoTeror::Points PyrLKTracker::track(GrayscaleImage prev, GrayscaleImage next, Points &prevPoints)
{
    std::vector<unsigned char> status;
    std::vector<float> errors;
    Points nextPoints = prevPoints;
    cv::calcOpticalFlowPyrLK(prev, next, prevPoints, nextPoints, status, errors);
    return nextPoints;
}
