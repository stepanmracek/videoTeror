#ifndef FARNEBACKMOTIONDETECTOR_H
#define FARNEBACKMOTIONDETECTOR_H

#include "motiondetector.h"

namespace VideoTeror
{
namespace MotionDetection
{

class FarnebackMotionDetector : MotionDetector
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

#endif // FARNEBACKMOTIONDETECTOR_H
