#ifndef VT_PROBABILISTICMODEL_H
#define VT_PROBABILISTICMODEL_H

#include <opencv2/core/core.hpp>
#include <QVector>

#include "vtsense.h"
#include "Helpers/serialization.h"

namespace VideoTeror
{
namespace MachineLearning
{

class ProbabilisticModel : public VideoTeror::Helpers::ISerializable
{
    cv::Ptr<cv::PCA> pca;
    int imageWidth;
    int imageHeight;

public:
    ProbabilisticModel();
    virtual ~ProbabilisticModel() { }

    virtual bool serialize(cv::FileStorage &storage);
    virtual bool deserialize(cv::FileStorage &storage);

    void learn(const QVector<VideoTeror::GrayscaleImage> &images);
    void show();

    GrayscaleImage generate(const MatF &params) const;
    MatF getParams(const GrayscaleImage &image) const;
    int getParamsSize() const { return pca->eigenvalues.rows; }
    float getEigenvalue(int i) const { return pca->eigenvalues.at<float>(i); }
};

}
}

#endif // VT_PROBABILISTICMODEL_H
