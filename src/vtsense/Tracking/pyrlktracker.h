#ifndef VT_PYRLKTRACKER_H
#define VT_PYRLKTRACKER_H

#include "tracker.h"

namespace VideoTeror
{
namespace Tracking
{

class PyrLKTracker : public Tracker
{
public:
    PyrLKTracker();

    Points track(BGRImage prev, BGRImage next, Points &prevPoints);
};

}
}
#endif // VT_PYRLKTRACKER_H
