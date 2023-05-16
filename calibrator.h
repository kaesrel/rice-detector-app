
#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include "img_utils.h"
#include<string>
#include<vector>
#include<QImage>

class Calibrator
{
public:
    Calibrator();
    ~Calibrator() {}
    bool loadCalibData();
    void saveCalibData();

    void calibrateInit();
    bool calibrateAddImage(const QImage &img);
    void calibrateFinished();

    int getErrId() const
    { return errid; }
    int getPictureCount() const
    { return pictureCount; }
    bool isValid() const
    { return mIsValid; }

    void unDistorted(QImage &image);

private:
    int pictureCount;
    std::string filename="calib.js";
    std::string errMesg;
    cv::Mat cameraMatrix,distCoefficients,R,T;
    bool mIsValid = false;
    std::vector<cv::Point3f> objp;
    std::vector<std::vector<cv::Point3f> > objpoints;
    std::vector<std::vector<cv::Point2f> > imgpoints;

    int w,h;
    int errid{0};
};

#endif // CALIBRATOR_H
