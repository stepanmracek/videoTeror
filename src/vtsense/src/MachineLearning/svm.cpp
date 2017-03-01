#include "vtsense/MachineLearning/svm.h"

using namespace VideoTeror::MachineLearning;

SVM::SVM()
{
    svm = cv::ml::SVM::create();
}

void SVM::learn(const std::vector<MatF> &samples, const std::vector<bool> &labels)
{
    unsigned int n = samples.size();
    if (n == 0) throw std::runtime_error("empty training set");
    if (n != labels.size()) throw std::runtime_error("samples and labels count mismatch");
    int l = samples[0].rows;

    MatF data(n, l);
    MatF responses(n, 1);
    for (unsigned int r = 0; r < n; r++)
    {
        responses(r) = labels[r] ? 1 : -1;
        for (int c = 0; c < l; c++)
        {
            data(r, c) = samples[r](c);
        }
    }

    svm->setType(cv::ml::SVM::C_SVC);
    svm->setKernel(cv::ml::SVM::LINEAR);
    svm->setTermCriteria(cv::TermCriteria(CV_TERMCRIT_ITER, 1000, 1e-6));
    svm->trainAuto(cv::ml::TrainData::create(data, cv::ml::COL_SAMPLE, labels));
}

float SVM::classify(const MatF &input)
{
    return svm->predict(input, cv::noArray(), cv::ml::StatModel::RAW_OUTPUT);
}

bool SVM::serialize(cv::FileStorage &storage)
{
    if (!storage.isOpened()) throw("FileStorage is not opened");
    svm->write(storage);
    return true;
}

bool SVM::deserialize(cv::FileStorage &storage)
{
    if (!storage.isOpened()) throw("FileStorage is not opened");
    //cv::ml::SVM::read(storage["myNodeName"])
    throw std::runtime_error("Not implemented for OpenCV 3.2");
    return true;
}
