#ifndef VT_GRADIENTMOTIONDETECTOR_H
#define VT_GRADIENTMOTIONDETECTOR_H

#include <QList>

#include "motiondetector.h"

namespace VideoTeror
{
namespace MotionDetection
{

class GradientMotionDetector : public MotionDetector
{
    int blockSize;

public:
    GradientMotionDetector(GrayscaleImage &initialFrame, int blockSize, int historyLength = 50);

    GrayscaleImage detect(const GrayscaleImage &curFrame);
};

}
}

#endif // VT_GRADIENTMOTIONDETECTOR_H
