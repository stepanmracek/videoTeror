#ifndef OPTICALFLOWMOTIONDETECTOR_H
#define OPTICALFLOWMOTIONDETECTOR_H

#include "motiondetector.h"
#include "common.h"

namespace VideoTeror
{
namespace MotionDetection
{

class OpticalFlowMotionDetector : MotionDetector
{
    GrayscaleImage prevFrame;
    int blockSize;
    //double threshold;

public:
    OpticalFlowMotionDetector(const GrayscaleImage &initialFrame, int blockSize/*, double threshold*/);

    GrayscaleImage detect(const GrayscaleImage &curFrame);
};

}
}

#endif // OPTICALFLOWMOTIONDETECTOR_H
