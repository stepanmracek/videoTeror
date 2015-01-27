#ifndef VT_BINARYCLASSIFIER_H
#define VT_BINARYCLASSIFIER_H

#include "vtsense/vtsense.h"
#include "vtsense/Helpers/serialization.h"

namespace VideoTeror
{
namespace MachineLearning
{

class BinaryClassifier : public Helpers::ISerializable
{
public:
    virtual void learn(const std::vector<VideoTeror::MatF> &samples, const std::vector<bool> &labels) = 0;
    virtual float classify(const VideoTeror::MatF &input) = 0;
};

}
}
#endif // VT_BINARYCLASSIFIER_H
