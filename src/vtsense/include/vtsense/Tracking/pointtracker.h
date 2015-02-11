#ifndef VT_POINTTRACKER_H
#define VT_POINTTRACKER_H

#include <vector>

#include "vtsense/vtsense.h"

namespace VideoTeror
{
namespace Tracking
{

class PointTracker
{
public:

    PointTracker() {}

    virtual VideoTeror::Points track(BGRImage prev, BGRImage next, Points &prevPoints) = 0;
};

}
}

#endif // VT_POINTTRACKER_H
