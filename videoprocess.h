#ifndef VIDEOPROCESS_H
#define VIDEOPROCESS_H
#include <opencv2/opencv.hpp>
using namespace cv;

class videoprocess
{
public:
    videoprocess();
    void start_video();
//    void doDefectDetection(cv::Mat frame);
};

#endif // VIDEOPROCESS_H
