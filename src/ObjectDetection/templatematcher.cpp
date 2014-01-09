#include "templatematcher.h"

using namespace VideoTeror::ObjectDetection;

TemplateMatcher::TemplateMatcher(const QVector<BGRImage> &templates) :
    templates(templates)
{
    resultMats = QVector<cv::Mat>(templates.count());
}

TemplateMatcher::Result TemplateMatcher::match(const BGRImage &frame)
{
    TemplateMatcher::Result result;
    result.score = -1e300;

    int tCount = templates.count();
    for (int i = 0; i < tCount; i++)
    {
        cv::matchTemplate(frame, templates[i], resultMats[i], CV_TM_CCOEFF_NORMED);
        double max;
        cv::Point maxLoc;
        cv::minMaxLoc(resultMats[i], NULL, &max, 0, &maxLoc);

        if (max > result.score)
        {
            result.score = max;
            result.location = cv::Point(maxLoc.x + templates[i].cols/2, maxLoc.y + templates[i].rows/2);
        }
    }

    return result;
}
