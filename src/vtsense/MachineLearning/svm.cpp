#include "svm.h"

using namespace VideoTeror::MachineLearning;

SVM::SVM()
{
    svm = new cv::SVM();
}

void SVM::learn(const QVector<VideoTeror::MatF> &samples, const QVector<bool> &labels)
{
    int n = samples.count();
    if (n == 0) throw std::runtime_error("empty training set");
    if (n != labels.count()) throw std::runtime_error("samples and labels count mismatch");
    int l = samples[0].rows;

    MatF data(n, l);
    MatF responses(n, 1);
    for (int r = 0; r < n; r++)
    {
        responses(r) = labels[r] ? 1 : -1;
        for (int c = 0; c < l; c++)
        {
            data(r, c) = samples[r](c);
        }
    }

    cv::SVMParams params;
    params.svm_type = cv::SVM::C_SVC;
    params.kernel_type = cv::SVM::LINEAR;
    params.term_crit   = cv::TermCriteria(CV_TERMCRIT_ITER, 1000, 1e-6);
    svm->train(data, responses, cv::Mat(), cv::Mat(), params);
}

float SVM::classify(const MatF &input)
{
    return svm->predict(input, true);
}

bool SVM::serialize(cv::FileStorage &storage)
{
    if (!storage.isOpened()) throw("FileStorage is not opened");
    svm->write(*storage, "svm");
    return true;
}

bool SVM::deserialize(cv::FileStorage &storage)
{
    if (!storage.isOpened()) throw("FileStorage is not opened");
    svm->read(*storage, *storage["svm"]);
    return true;

}
