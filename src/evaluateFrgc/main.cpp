#include <biometrics/multiextractor.h>
#include <linalg/loader.h>

int main()
{
    /*std::string path = "/mnt/data/face/2d/aligned/";
    Face::Biometrics::MultiExtractor extractor("../../data/2d-extractor-2");

    std::vector<std::string> files = Face::LinAlg::Loader::listFiles(path, "*.png", Face::LinAlg::Loader::Filename);
    std::vector<Face::Biometrics::MultiTemplate> templates(files.size() - 800);

    #pragma omp parallel for
    for (int i = 800; i < files.size(); i++)
    {
        int id = 0; files[i].split('d')[0].toInt();
        ImageGrayscale img = cv::imread((path + files[i]).toStdString(), cv::IMREAD_GRAYSCALE);
        templates[i - 800] = extractor.extract(Face::Biometrics::MultiExtractor::ImageData(img), 1, id);
        //qDebug() << i << "/" << files.count();
    }

    auto eval = extractor.evaluate(templates);
    eval.printStats();
    eval.outputResults("../../data/frgc", 50);*/
}
