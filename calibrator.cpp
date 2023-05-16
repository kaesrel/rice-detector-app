#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/calib3d/calib3d_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include <string>
#include <QString>

#include "calibrator.h"

using namespace cv;
using namespace std;

Calibrator::Calibrator()
{
    errid=0;

    loadCalibData();
}

bool Calibrator::loadCalibData()
{
    mIsValid = false;
    FileStorage fs(filename, cv::FileStorage::READ);
    if(!fs.isOpened())
        return false;
    mIsValid = true;
    fs["cameraMatrix"] >> this->cameraMatrix;
    fs["distCoefficients"] >>  this->distCoefficients;
    fs.release();
    return true;
}

void Calibrator::saveCalibData()
{
    if(!mIsValid)
        return;
    FileStorage fs(filename, cv::FileStorage::WRITE);
    if(!fs.isOpened())
        return;
    fs << "cameraMatrix" << this->cameraMatrix;
    fs << "distCoefficients" <<  this->distCoefficients;
    fs.release();
    return;
}

void Calibrator::calibrateInit()
{
    objpoints.clear();
    imgpoints.clear();
    objp.clear();

    for( int i = 0; i < boardSize.height; ++i )
        for( int j = 0; j < boardSize.width; ++j )
            objp.push_back(Point3f(j, i, 0));
    pictureCount = 0;
    return;
}

bool Calibrator::calibrateAddImage(const QImage &img)
{
    errid=0;
    QImage img_(img);
    scaleImageDown(img_);
    if(img_.width() < img_.height())
    {
        // errid=1;
        // return false;

        QTransform tmat;
        tmat.rotate(-90.0);
        img_ = img_.transformed(tmat);
    }

    Mat image = QImageToCvMat(img_);
    Mat gray;

    cvtColor(image,gray,cv::COLOR_BGR2GRAY);
    std::vector<cv::Point2f> corners;
    bool success;
    success = findChessboardCorners(gray, boardSize, corners,
                CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK
                | CV_CALIB_CB_NORMALIZE_IMAGE);
    if(!success)
    {
        errid=2;
        return false;
    }
    h = gray.rows;
    w = gray.cols;
    TermCriteria criteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.001);
    cornerSubPix(gray,corners,cv::Size(11,11), cv::Size(-1,-1),criteria);

    objpoints.push_back(objp);
    imgpoints.push_back(corners);
    pictureCount++;

    return true;
}

void Calibrator::calibrateFinished()
{
    mIsValid = true;
    cv::calibrateCamera(objpoints, imgpoints, cv::Size(h,w),
        cameraMatrix, distCoefficients, R, T);
    saveCalibData();
}

void Calibrator::unDistorted(QImage &image)
{
    if(!mIsValid)
        return;
    Mat img = QImageToCvMat(image);
    Mat imgUndist;
    Mat newCameraMtx = getOptimalNewCameraMatrix(cameraMatrix,distCoefficients,
                              Size(image.width(),image.height()),1);
    undistort(img,imgUndist,cameraMatrix,distCoefficients,newCameraMtx);
    image = cvMatToQImage(imgUndist);
    return;
}
