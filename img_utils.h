
#ifndef IMG_UTILS_H
#define IMG_UTILS_H

#include <opencv2/core.hpp>
#include <QString>
#include <QPixmap>
#include <QImage>
//#include <QString>
//#include <QVector>


//const int checkerBoardSize[2] = {5,7};
const cv::Size boardSize{5,7};

QImage cvMatToQImage( const cv::Mat &inMat );
QPixmap cvMatToQPixmap( const cv::Mat &inMat );
cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true );
cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData = true );
void scaleImageDown(QImage &image,int maxSize=1024);
void scaleImageDown(QPixmap &image,int maxSize=1024);
#endif // IMG_UTILS_H
