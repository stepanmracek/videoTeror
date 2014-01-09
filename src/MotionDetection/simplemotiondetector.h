#ifndef SIMPLEMOTIONDETECTOR_H
#define SIMPLEMOTIONDETECTOR_H

#include <QList>
#include "motiondetector.h"

namespace VideoTeror
{
namespace MotionDetection
{

class SimpleMotionDetector : MotionDetector
{
    int blockSize;
    unsigned char threshold;

public:
    SimpleMotionDetector(GrayscaleImage &initialFrame, int blockSize, int threshold, int historyLength = 50);

    virtual GrayscaleImage detect(const GrayscaleImage &curFrame);
};

}
}

#endif // SIMPLEMOTIONDETECTOR_H
