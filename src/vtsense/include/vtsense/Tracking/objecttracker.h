#ifndef VT_OBJECTTRACKER_H
#define VT_OBJECTTRACKER_H

#include <functional>
#include <atomic>
#include <stdexcept>

#include "vtsense/vtsense.h"
#include "vtsense/Tracking/pyrlktracker.h"
#include "vtsense/ObjectDetection/objectdetector.h"
#include "vtsense/Tracking/trajectory.h"

namespace VideoTeror
{
namespace Tracking
{

class ObjectTracker
{
public:

    struct Settings
    {
        Settings()
        {
            forgetThreshold = 30;
            maxMoveThreshold = 100;
        }

        int forgetThreshold;
        double maxMoveThreshold;
    };

    struct Result
    {
        std::map<int, Trajectory> trajectories;
        std::map<int, ObjectDetection::ObjectDetector::DetectionResult::vector> objectsPerFrame;

        VideoTeror::GrayscaleImage drawTrajectories(const cv::Size &size) const;
    };

    class ComputationCanceled : public std::runtime_error
    {
    public:
        explicit ComputationCanceled(const std::string &what) : std::runtime_error(what) {}
    };

    ObjectTracker(ObjectDetection::ObjectDetector &detector, const Settings &settings = Settings());

    Result detectAndTrack(cv::VideoCapture &source, std::function<void(int)> progress = 0);
    void detectAndTrack(const VideoTeror::BGRImage &prevFrame, const VideoTeror::BGRImage &currentFrame,
                        int frameIndex, Result &result);

    void cancel() { stopRequest = true; }

private:

    Settings settings;
    ObjectDetection::ObjectDetector &detector;
    PyrLKTracker pointTracker;

    std::vector<int> missCounter;
    std::vector<int> pointIds;
    VideoTeror::Points trackingPoints;
    std::atomic_bool stopRequest;


    void cleanUpPoints(const std::vector<int> &toRemove);
    int getTrackedPointIndex(const ObjectDetection::ObjectDetector::DetectionResult &detectedObject, const cv::Size &frameSize);
};

}
}

#endif // VT_OBJECTTRACKER_H
