#ifndef VT_SIMPLEMOTIONDETECTOR_H
#define VT_SIMPLEMOTIONDETECTOR_H

#include <QList>
#include "motiondetector.h"

namespace VideoTeror
{
namespace MotionDetection
{

class SimpleMotionDetector : public MotionDetector
{
    int blockSize;
    unsigned char threshold;

public:
    SimpleMotionDetector(GrayscaleImage &initialFrame, int blockSize, int threshold, int historyLength = 50);

    virtual GrayscaleImage detect(const GrayscaleImage &curFrame);
};

}
}

#endif // VT_SIMPLEMOTIONDETECTOR_H
