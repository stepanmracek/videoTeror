#ifndef VT_PYRLKTRACKER_H
#define VT_PYRLKTRACKER_H

#include "pointtracker.h"

namespace VideoTeror
{
namespace Tracking
{

class PyrLKTracker : public PointTracker
{
public:
    PyrLKTracker();

    Points track(BGRImage prev, BGRImage next, Points &prevPoints);
};

}
}
#endif // VT_PYRLKTRACKER_H
