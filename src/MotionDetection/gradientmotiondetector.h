#ifndef GRADIENTMOTIONDETECTOR_H
#define GRADIENTMOTIONDETECTOR_H

#include <QList>

#include "motiondetector.h"

namespace VideoTeror
{
namespace MotionDetection
{

class GradientMotionDetector : MotionDetector
{
    int blockSize;

public:
    GradientMotionDetector(GrayscaleImage &initialFrame, int blockSize, int historyLength = 50);

    GrayscaleImage detect(const GrayscaleImage &curFrame);
};

}
}

#endif // GRADIENTMOTIONDETECTOR_H
