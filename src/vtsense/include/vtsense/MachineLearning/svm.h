#ifndef VT_SVM_H
#define VT_SVM_H

#include "binaryclassifier.h"

namespace VideoTeror
{
namespace MachineLearning
{

class SVM : public BinaryClassifier
{
protected:
    cv::Ptr<cv::SVM> svm;

public:
    SVM();
    void learn(const std::vector<VideoTeror::MatF> &samples, const std::vector<bool> &labels);
    float classify(const MatF &input);

    bool serialize(cv::FileStorage &storage);
    bool deserialize(cv::FileStorage &storage);
};

}
}

#endif // VT_SVM_H
