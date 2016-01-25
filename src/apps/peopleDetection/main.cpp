#include <QApplication>
#include <QInputDialog>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include "vtsense/ObjectDetection/hogpeopledetector.h"
#include "vtsense/Tracking/pyrlktracker.h"
#include "vtsense/Helpers/helpers.h"
#include "vtsenseExtras/dircrawler.h"
#include "vtsense/Tracking/objecttracker.h"

void cleanUpPoints(VideoTeror::Points &points, QVector<int> &missCounter, const QVector<int> &toRemove)
{
	VideoTeror::Points newPoints;
	QVector<int> newMissCounter;

	for (int i = 0; i < points.size(); i++)
	{
		if (!toRemove.contains(i))
		{
			newPoints.push_back(points[i]);
			newMissCounter.push_back(missCounter[i]);
		}
	}

	points = newPoints;
	missCounter = newMissCounter;
}

int main(int argc, char *argv[])
{
	QStringList filters; filters << "*.mov";
	QDir dir("/media/data/videoteror/people/");
	QStringList videos = VideoTeror::Extras::DirCrawler::recursiveSearch(dir, filters);

	QApplication app(argc, argv);
	bool ok;
	QString selection = QInputDialog::getItem(NULL, "Select video", "video:", videos, 0, false, &ok);
	if (!ok) return 0;

	cv::VideoCapture video(selection.toStdString());

	VideoTeror::ObjectDetection::HogPeopleDetector detector(1.0, 3.0);
	VideoTeror::Tracking::ObjectTracker tracker(detector);
	VideoTeror::Tracking::ObjectTracker::Result result;

	int index = 0;
	VideoTeror::BGRImage frame, prev, gui;
	cv::Scalar green(0, 255, 0);
	video.read(prev);
	cv::Size s(prev.cols, prev.rows);

	/*cv::VideoWriter videoWriter("peopleDetection.avi", CV_FOURCC('M','P','4','V'), video.get(CV_CAP_PROP_FPS), prev.size());
	if (!videoWriter.isOpened())
	{
		throw std::runtime_error("Can't write to output video");
	}*/

	index++;
	char key;
	while (video.read(frame) && (key = cv::waitKey(1)) != 27)
	{
		tracker.detectAndTrack(prev, frame, index, result);
		frame.copyTo(gui);

		for (const VideoTeror::ObjectDetection::ObjectDetector::DetectionResult &o : result.objectsPerFrame[index])
		{
			cv::rectangle(gui, o.toPixelRegion(s), green, 3);
			cv::circle(gui, o.toPixelPoint(s), 3, green, 1, CV_AA);
			cv::putText(gui, std::to_string(o.id), o.toPixelRegion(s).tl() + cv::Point(0, 15),
						cv::FONT_HERSHEY_SIMPLEX, 0.5, green, 1, CV_AA);
		}

		frame.copyTo(prev);

		cv::imshow("video", gui);
		if (key == 's')
		{
			cv::imwrite("people-detection-" + std::to_string(index) + ".png", gui);
		}

		//videoWriter << gui;

		index++;
	}

	VideoTeror::GrayscaleImage trajectories = result.drawTrajectories(s);
	cv::imshow("trajectories", trajectories);
	cv::waitKey(0);
	cv::imwrite("people-trajectories.png", trajectories);

	return 0;
}
