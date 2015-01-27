#include <string>

#include <faceCommon/biometrics/multiextractor.h>
#include <faceCommon/linalg/loader.h>

int main()
{
    std::string path = "/mnt/data/face/2d/aligned/";
    Face::Biometrics::MultiExtractor extractor("../../data/2d-extractor-2");

    std::vector<std::string> files = Face::LinAlg::Loader::listFiles(path, "*.png", Face::LinAlg::Loader::Filename);
    std::vector<Face::Biometrics::MultiTemplate> templates(files.size() - 800);

    #pragma omp parallel for
    for (int i = 800; i < files.size(); i++)
    {
        //files[i].split('d')[0].toInt();
        int id = atoi(files[i].substr(0, files[i].find('d')).c_str());

        ImageGrayscale img = cv::imread((path + files[i]), cv::IMREAD_GRAYSCALE);
        templates[i - 800] = extractor.extract(Face::Biometrics::MultiExtractor::ImageData(img), 1, id);
        //qDebug() << i << "/" << files.count();
    }

    auto eval = extractor.evaluate(templates);
    eval.printStats();
    eval.outputResults("../../data/frgc", 50);
}
