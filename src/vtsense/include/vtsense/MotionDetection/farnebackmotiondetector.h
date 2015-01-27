#ifndef VT_FARNEBACKMOTIONDETECTOR_H
#define VT_FARNEBACKMOTIONDETECTOR_H

#include "motiondetector.h"

namespace VideoTeror
{
namespace MotionDetection
{

class FarnebackMotionDetector : public MotionDetector
{
    GrayscaleImage prevFrame;
    int blockSize;
    //double threshold;

public:
    FarnebackMotionDetector(const GrayscaleImage &initialFrame, int blockSize/*, double threshold*/);

    GrayscaleImage detect(const GrayscaleImage &curFrame);
};

}
}

#endif // VT_FARNEBACKMOTIONDETECTOR_H
