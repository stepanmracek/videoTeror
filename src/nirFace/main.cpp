#include <OpenNI.h>
#include <QDebug>

#include "vtsense.h"

int WIDTH = 640;
int HEIGHT = 480;

class NewFrameListener : public openni::VideoStream::NewFrameListener
{
    openni::VideoFrameRef frameRef;
public:
    VideoTeror::GrayscaleImage frame;

    NewFrameListener()
    {
        frame = VideoTeror::GrayscaleImage::zeros(HEIGHT, WIDTH);
    }


    void onNewFrame(openni::VideoStream &stream)
    {
        if (!stream.isValid()) return;
        if (stream.readFrame(&frameRef) != openni::STATUS_OK) return;

        //openni::Grayscale16Pixel max = 0;
        const openni::Grayscale16Pixel* data = (const openni::Grayscale16Pixel*)(frameRef.getData());
        /*for (int i = 0; i < WIDTH*HEIGHT; i++)
        {
            if (data[i] > max) max = data[i];
        }*/
        for (int r = 0; r < HEIGHT; r++)
        {
            for (int c = 0; c < WIDTH; c++)
            {
                //frame(r, c) = data[WIDTH*r + c]*255.0/max;
                double v = data[WIDTH*r + c]*255.0/1000.0;
                if (v > 255) v = 255;
                frame(r, c) = v;
            }
        }
    }
};

int main(int argc, char *argv[])
{
    qDebug() << "init:" << openni::OpenNI::initialize();

    openni::Device device;
    qDebug() << "device open:" << device.open(openni::ANY_DEVICE);
    openni::DeviceInfo devInfo = device.getDeviceInfo();
    qDebug() << devInfo.getVendor() << devInfo.getName() << "@" << devInfo.getUri();

    const openni::SensorInfo *sensorIrInfo = device.getSensorInfo(openni::SENSOR_IR);
    const openni::Array<openni::VideoMode> &irVideoModes = sensorIrInfo->getSupportedVideoModes();

    int selectedMode = -1;
    for (int i = 0; i < irVideoModes.getSize(); i++)
    {
        const openni::VideoMode &vm = irVideoModes[i];
        qDebug() << vm.getResolutionX() << "x" << vm.getResolutionY() << "@" << vm.getFps() << vm.getPixelFormat();
        if (vm.getResolutionX() == WIDTH && vm.getResolutionY() == HEIGHT &&
            vm.getFps() == 30 && vm.getPixelFormat() == openni::PIXEL_FORMAT_GRAY16)
            selectedMode = i;
    }

    openni::VideoStream stream;
    qDebug() << "create stream:" << stream.create(device, openni::SENSOR_IR);

    NewFrameListener listener;
    qDebug() << "add new frame listener:" << stream.addNewFrameListener(&listener);
    qDebug() << "set video mode:" << stream.setVideoMode(irVideoModes[selectedMode]);
    qDebug() << "stream start:" << stream.start();

    char key;
    VideoTeror::GrayscaleImage equalized;
    VideoTeror::GrayscaleImage clahed;
    auto clahe = cv::createCLAHE();
    while ((key = cv::waitKey(1)) != 27)
    {
        cv::equalizeHist(listener.frame, equalized);
        clahe->apply(listener.frame, clahed);

        cv::imshow("frame", listener.frame);
        cv::imshow("eq", equalized);
        cv::imshow("clahe", clahed);
    }

    stream.stop();
    device.close();
    openni::OpenNI::shutdown();
}
