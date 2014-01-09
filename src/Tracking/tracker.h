#ifndef TRACKER_H
#define TRACKER_H

#include <vector>

#include "common.h"

namespace VideoTeror
{
namespace Tracking
{

typedef std::vector<cv::Point2f> Points;

class Tracker
{
public:

    Tracker();

    virtual Points track(BGRImage prev, BGRImage next, Points &prevPoints) = 0;
};

}
}

#endif // TRACKER_H
