#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/calib3d/calib3d_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "warper.h"
#include <vector>
#include <QDebug>

using namespace std;
using namespace cv;

Warper::Warper()
{

}

bool Warper::warp(const QPixmap &inPixmap)
{
    Mat image=QPixmapToCvMat(inPixmap);
    Mat gray;
    cvtColor(image,gray,cv::COLOR_BGR2GRAY);
    std::vector<cv::Point2f> corners;
    bool success;
    success = findChessboardCorners(gray, boardSize, corners,
                                    CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK
                                        | CV_CALIB_CB_NORMALIZE_IMAGE);
    if(!success)
        return false;
    int h = gray.rows;
    int w = gray.cols;
    TermCriteria criteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.001);
    cornerSubPix(gray,corners,cv::Size(11,11), cv::Size(-1,-1),criteria);

//    const cv::Size boardSize{5,7};

    int nh,nw;
    nh = boardSize.width;
    nw = boardSize.height;

    vector<cv::Point2f> sMat {
        corners[0],
        corners[nh-1],
        corners[corners.size() - nh],
        corners[corners.size() - 1],
    };


    for(int i=0; size_t(i) < corners.size(); i++)
    {
        qDebug() << "corners" << i << ":" << corners[i].x << corners[i].y;
    }

    for(int i=0; size_t(i) < sMat.size(); i++)
    {
        qDebug() << "sMat" << i << ":" << sMat[i].x << sMat[i].y;
    }

    nh -= 1;
    nw -= 1;

    float sumx=0, sumy=0;
    for(auto p:sMat)
    {
        sumx += p.x;
        sumy += p.y;
    }

    // Center
    float xc = sumx/sMat.size();
    float yc = sumy/sMat.size();

    // deltas
    float dx = ((-sMat[2].x + sMat[0].x - sMat[3].x + sMat[1].x)/2) / nw; // Ratio x
    float dy = ((-sMat[2].y + sMat[3].y - sMat[0].y + sMat[1].y)/2) / nh; // Ratio y

    float dt = (dx + dy) / 2;  // Ratio average
    float dcx = (dt*nw) / 2;  // Center distance
    float dcy = (dt*nh) / 2;

    vector<cv::Point2f> dMat {
        {xc + dcx, yc - dcy},
        {xc + dcx, yc + dcy},
        {xc - dcx, yc - dcy},
        {xc - dcx, yc + dcy}
    };

    Mat M = getPerspectiveTransform(sMat, dMat);
    Mat outImage;
    image.copyTo(outImage);
    warpPerspective(image, outImage, M, Size(w,h), cv::INTER_LINEAR);
    outPixmap = cvMatToQPixmap(outImage);

    return true;
}
