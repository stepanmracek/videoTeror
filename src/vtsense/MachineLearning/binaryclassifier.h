#ifndef VT_BINARYCLASSIFIER_H
#define VT_BINARYCLASSIFIER_H

#include <QVector>

#include "vtsense.h"
#include "Helpers/serialization.h"

namespace VideoTeror
{
namespace MachineLearning
{

class BinaryClassifier : public Helpers::ISerializable
{
public:
    virtual void learn(const QVector<VideoTeror::MatF> &samples, const QVector<bool> &labels) = 0;
    virtual float classify(const VideoTeror::MatF &input) = 0;
};

}
}
#endif // VT_BINARYCLASSIFIER_H
