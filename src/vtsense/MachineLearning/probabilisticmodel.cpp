#include "probabilisticmodel.h"

#include <QString>
#include <QDebug>

using namespace VideoTeror::MachineLearning;

ProbabilisticModel::ProbabilisticModel()
{
    pca = new cv::PCA();
}

bool ProbabilisticModel::serialize(cv::FileStorage &storage)
{
    if (!storage.isOpened()) throw("FileStorage is not opened");
    storage << "mean" << pca->mean;
    storage << "eigenvalues" << pca->eigenvalues;
    storage << "eigenvectors" << pca->eigenvectors;
    storage << "imageWidth" << imageWidth;
    storage << "imageHeight" << imageHeight;
    return true;
}

bool ProbabilisticModel::deserialize(cv::FileStorage &storage)
{
    if (!storage.isOpened()) throw("FileStorage is not opened");
    storage["mean"] >> pca->mean;
    storage["eigenvalues"] >> pca->eigenvalues;
    storage["eigenvectors"] >> pca->eigenvectors;
    storage["imageWidth"] >> imageWidth;
    storage["imageHeight"] >> imageHeight;
    return true;
}

void ProbabilisticModel::learn(const QVector<VideoTeror::GrayscaleImage> &images)
{
    if (images.count() == 0) throw std::runtime_error("empty training set");
    imageWidth = images[0].cols;
    imageHeight = images[0].rows;

    VideoTeror::MatF data(images[0].cols * images[0].rows, images.count());
    for (int i = 0; i < images.count(); i++)
    {
        if (images[i].cols != imageWidth || images[i].rows != imageHeight)
        {
            throw std::runtime_error("train sample #" + std::to_string(i) + " has different size");
        }
        GrayscaleImage reshaped = images[i].reshape(1, images[i].rows * images[i].cols);
        for (int j = 0; j < reshaped.rows; j++)
        {
            data(j, i) = reshaped(j)/255.0f;
        }
    }
    (*pca)(data, cv::Mat(), CV_PCA_DATA_AS_COL);
}

VideoTeror::GrayscaleImage ProbabilisticModel::generate(const MatF &params) const
{
    return pca->backProject(params).reshape(1, imageHeight)*255;
}

VideoTeror::MatF ProbabilisticModel::getParams(const GrayscaleImage &image) const
{
    MatF input;
    if (image.rows != imageHeight || image.cols != imageWidth)
    {
        GrayscaleImage newImage;
        cv::resize(image, newImage, cv::Size(imageWidth, imageHeight));
        input = newImage.reshape(1, imageWidth*imageHeight);
    }
    else
    {
        input = image.reshape(1, imageWidth*imageHeight);
    }
    input /= 255.0f;
    return pca->project(input);
}

void ProbabilisticModel_onTrackbarChange(int pos, void* userdata)
{
    ProbabilisticModel &model = *(ProbabilisticModel*)userdata;

    VideoTeror::MatF params = VideoTeror::MatF::zeros(model.getParamsSize(), 1);
    for (int i = 0; i < 10; i++)
    {
        float v = cv::getTrackbarPos(QString::number(i).toStdString(), "model") - 5.0f;
        v = v * 1 * sqrt(model.getEigenvalue(i));
        params(i) = v;
    }

    VideoTeror::GrayscaleImage image = model.generate(params);
    cv::imshow("model", image);
    cv::waitKey(1);
}

int ProbabilisticModel_tbValues[10];

void ProbabilisticModel::show()
{
    cv::namedWindow("model", CV_WINDOW_NORMAL);
    cv::resizeWindow("model", 200, 200);

    for (int i = 0; i < 10; i++)
    {
        ProbabilisticModel_tbValues[i] = 5;
        cv::createTrackbar(QString::number(i).toStdString(), "model",
                           &ProbabilisticModel_tbValues[i], 11, ProbabilisticModel_onTrackbarChange, this);
    }

    cv::imshow("model", pca->mean.reshape(1, imageHeight));
    while ((char)cv::waitKey() != 27) { }
    cv::destroyWindow("model");
}
