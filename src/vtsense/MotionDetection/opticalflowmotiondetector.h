#ifndef VT_OPTICALFLOWMOTIONDETECTOR_H
#define VT_OPTICALFLOWMOTIONDETECTOR_H

#include "motiondetector.h"
#include "vtsense.h"

namespace VideoTeror
{
namespace MotionDetection
{

class OpticalFlowMotionDetector : public MotionDetector
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

#endif // VT_OPTICALFLOWMOTIONDETECTOR_H
