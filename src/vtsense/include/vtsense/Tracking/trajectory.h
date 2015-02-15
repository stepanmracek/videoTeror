#ifndef VT_TRAJECTORY_H
#define VT_TRAJECTORY_H

#include <opencv2/core/core.hpp>

namespace VideoTeror
{
namespace Tracking
{

struct Trajectory
{
    Trajectory(int frameStart = 0) : frameStart(frameStart) { }

    Trajectory(int frameStart, cv::Point2d initialPoint) : frameStart(frameStart)
    {
        points.push_back(initialPoint);
    }

    int frameStart;
    std::vector<cv::Point2d> points;
};

}
}

#endif
