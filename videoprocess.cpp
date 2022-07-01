#include "videoprocess.h"
#include <QDebug>
videoprocess::videoprocess()
{

}

void doDefectDetection(cv::Mat frame){
    qDebug()<<"image processd\n";

}
void videoprocess::start_video(){
    VideoCapture cap("/home/cv-startup/projectAI/Qt/textile/test.mp4");

    if(!cap.isOpened()){
        qDebug() << "Error opening video stream or file" << endl;
        return;
      }

    cv::Mat inFrame;
    while(1){
        cap>>inFrame;
        if(inFrame.empty()){
            qInfo("frame is empty..\n");
            break;
            continue;
           }
        doDefectDetection(inFrame);

        cv::imshow( "Frame", inFrame );

            // Press  ESC on keyboard to exit
         char c=(char)waitKey(10);
         if(c==27)
             break;

//        inDisplay(
//               QPixmap::fromImage(
//                   QImage(
//                       inFrame.data,
//                       inFrame.cols,
//                       inFrame.rows,
//                       inFrame.step,
//                       QImage::Format_RGB888)
//                   .rgbSwapped()));

    }
    cap.release();

     // Closes all the frames
    destroyAllWindows();
}
