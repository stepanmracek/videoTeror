#include <QApplication>
#include <QDir>
#include <QInputDialog>
#include <opencv2/opencv.hpp>
#include <functional>

#include "vtsense/ObjectDetection/haardetector.h"
#include "vtsense/Tracking/objecttracker.h"
#include "vtsenseExtras/dircrawler.h"
#include "vtsense/ObjectDetection/dlibfacedetector.h"

void progressFunc(int percent)
{
	std::cout << percent << std::endl;
}

class ProgressReporter
{
public:
	VideoTeror::Tracking::ObjectTracker &tracker;

	ProgressReporter(VideoTeror::Tracking::ObjectTracker &tracker) : tracker(tracker) {}

	void callbackFunction(int percent)
	{
		std::cout << percent << std::endl;

		if (percent >= 5)
		{
			tracker.cancel();
		}
	}
};

int main(int argc, char *argv[])
{
	QStringList filters; filters << "*.mov";
	QDir dir("/media/data/videoteror/face");
	QStringList videos = VideoTeror::Extras::DirCrawler::recursiveSearch(dir, filters);

	QApplication app(argc, argv);
	bool ok;
	QString selection = QInputDialog::getItem(NULL, "Select video", "video:", videos, 0, false, &ok);
	if (!ok) return 0;

	cv::VideoCapture video(selection.toStdString());

	cv::Scalar green(0, 255, 0);

	//std::string cascadeFile = "/home/stepo/Stažené/cascadePUT.xml";
	std::string cascadeFile = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt_tree.xml";
	//std::string cascadeFile = "/usr/share/opencv/lbpcascades/lbpcascade_frontalface.xml";
	//std::string cascadeFile = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml";
	VideoTeror::ObjectDetection::HaarDetector detector(cascadeFile, 0.5, cv::Size(8,8), cv::Size(96,96));
	//VideoTeror::ObjectDetection::DlibFaceDetector detector(0.5);

	VideoTeror::Tracking::ObjectTracker::Settings trackerSettings;
	//trackerSettings.forgetThreshold = 1;
	VideoTeror::Tracking::ObjectTracker tracker(detector, trackerSettings);
	VideoTeror::Tracking::ObjectTracker::Result result;

	int index = 0;
	VideoTeror::BGRImage frame, prev, gui;
	video.read(prev);
	cv::Size s(prev.cols, prev.rows);

	cv::VideoWriter videoWriter("face-detection.avi", CV_FOURCC('M','P','4','V'), video.get(CV_CAP_PROP_FPS), s);
	if (!videoWriter.isOpened())
	{
		throw std::runtime_error("Can't write to output video");
	}

	//result = tracker.detectAndTrack(video, progressFunc);

	ProgressReporter reporter(tracker);
	//result = tracker.detectAndTrack(video, std::bind(&ProgressReporter::callbackFunction, &reporter, std::placeholders::_1));

	index++;
	char key;
	while (video.read(frame) && (key = cv::waitKey(1)) != 27)
	{
		tracker.detectAndTrack(prev, frame, index, result);
		frame.copyTo(gui);

		for (const VideoTeror::ObjectDetection::ObjectDetector::DetectionResult &o : result.objectsPerFrame[index])
		{
			cv::rectangle(gui, o.toPixelRegion(s), green, 2);
			cv::circle(gui, o.toPixelPoint(s), 3, green, 1, CV_AA);
			cv::putText(gui, std::to_string(o.id), o.toPixelRegion(s).tl() + cv::Point(0, 15),
						cv::FONT_HERSHEY_SIMPLEX, 0.5, green, 1, CV_AA);
		}

		frame.copyTo(prev);

		videoWriter << gui;

		cv::imshow("video", gui);
		videoWriter << gui;
		if (key == 's')
		{
			cv::imwrite("face-detection-" + std::to_string(index) + ".png", gui);
		}

		index++;
	}

	VideoTeror::GrayscaleImage trajectories = result.drawTrajectories(s);
	cv::imshow("trajectories", trajectories);
	cv::waitKey(0);
	cv::imwrite("face-trajectories.png", trajectories);

	return 0;
}
