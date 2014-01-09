#ifndef PYRLKTRACKER_H
#define PYRLKTRACKER_H

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
#endif // PYRLKTRACKER_H
