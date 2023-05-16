#include "yolo_darknet.h"
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <vector>
#include <QDebug>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "img_utils.h"

using namespace std;
using namespace cv;
using namespace dnn;

static float colors[6][3] = { {1,0,1}, {0,0,1},{0,1,1},{0,1,0},{1,1,0},{1,0,0} };

static float get_color(int c, int x, int max)
{
    float ratio = (float(x)/max)*5;
    int i = int(floor(ratio));
    int j = int(ceil(ratio));
    ratio -= i;
    float r = (1-ratio) * colors[i][c] + ratio*colors[j][c];
    //printf("%f\n", r);
    return r;
}

yolo_darknet::yolo_darknet() :
    status(0),netWidth(416),netHeight(416)
{
//    net=readNetFromDarknet()
}

void yolo_darknet::loadConfig(const QString &classFile, const QString &configFile, const QString &weightFile,
                              float threshold,float nms)
{
    status=0;


    QFile cFile(classFile);
    if(!cFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << classFile << " is unable to open.";
        return;
    }

    QTextStream cStrm(&cFile);
    QString line;
    classNames.clear();
    classColors.clear();

    this->threshold=threshold;
    this->nms=nms;

    while(cStrm.readLineInto(&line))
    {
        classNames.append(line);
    }


    for(int i=0;i<classNames.size();i++)
    {
        int classes=classNames.size();
        int offset = i * 123457 % classes;
        int red = 255*get_color(2, offset, classes);
        int green = 255*get_color(1, offset, classes);
        int blue = 255*get_color(0, offset, classes);
        Scalar s(blue,green,red); //BGR
        classColors.push_back(s);
    }
//    qDebug() << "Going to readNetFromDarknet";
//    QSettings config(configFile,QSettings::IniFormat);
//    netWidth=config.value("net/width").toInt();
//    netHeight=config.value("net/height").toInt();

//    netWidth = netHeight = 416;
    qDebug() << netWidth << netHeight;

    net=readNetFromDarknet(configFile.toStdString(),weightFile.toStdString());
    net.setPreferableBackend(DNN_BACKEND_OPENCV);
    net.setPreferableTarget(DNN_TARGET_CPU);
    outNames = net.getUnconnectedOutLayersNames();

    status=1;
    qDebug() << __FUNCTION__;
}

void yolo_darknet::process(const QPixmap &input)
{
    objects.clear();
    if(status==0)
        return;
    qDebug() << __FUNCTION__;
//    Size size;
    imgSrc=QPixmapToCvMat(input);
//    size.width=imgSrc.cols;
//    size.height=imgSrc.rows;
//    Mat blob=blobFromImage(imgSrc,1/255.0,size,Scalar(0,0,0),true);
    Mat blob=blobFromImage(imgSrc,1/255.0,Size(netWidth,netHeight),Scalar(0,0,0),true);

    net.setInput(blob);
    std::vector<Mat> outs;

    net.forward(outs, outNames);

    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<Rect> boxes;

    for (size_t i = 0; i < outs.size(); ++i)
    {
        // Network produces output blob with a shape NxC where N is a number of
        // detected objects and C is a number of classes + 4 where the first 4
        // numbers are [center_x, center_y, width, height]
        float* data = reinterpret_cast<float*>(outs[i].data);
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            minMaxLoc(scores, nullptr, &confidence, nullptr, &classIdPoint);
            if (float(confidence) > threshold)
            {
                int centerX = int(data[0] * imgSrc.cols);
                int centerY = int(data[1] * imgSrc.rows);
                int width = int(data[2] * imgSrc.cols);
                int height = int(data[3] * imgSrc.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back(float(confidence));
                boxes.push_back(Rect(left, top, width, height));
            }
        }
    }

    imgSrc.copyTo(imgRes);
    qDebug() << "threshold=" << threshold << " nms=" << nms;

    std::vector<int> indices;
    NMSBoxes(boxes, confidences, threshold, nms, indices);

    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        Rect box = boxes[idx];
        objects.push_back(yolo_obj(classIds[idx],confidences[idx],box));
        drawPred(classIds[idx], confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, imgRes);
    }
//    imshow("",imgRes);
}

void yolo_darknet::drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame)
{
    rectangle(frame, Point(left, top), Point(right, bottom), classColors[classId]);

    std::string label = format("%.2f", conf);
    if (!classNames.empty())
    {
        CV_Assert(classId < (int)classNames.size());
        label = classNames[classId].toStdString() + ": " + label;
    }

    int baseLine;
    double font_size = double(frame.rows / 1000.F);
    font_size=(font_size<0.25)?0.25:font_size;

    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, font_size, 1, &baseLine);

    top = max(top, labelSize.height);
    rectangle(frame, Point(left, top - labelSize.height),
              Point(left + labelSize.width, top + baseLine), classColors[classId], FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, font_size, Scalar());
    return;
}

const QPixmap &yolo_darknet::getPixmap() const
{
    pixRes=cvMatToQPixmap(imgRes);
    return(pixRes);
}

//QImage yolo_darknet::cvMatToQImage( const cv::Mat &inMat )
//{
//   switch ( inMat.type() )
//   {
//   // 8-bit, 4 channel
//   case CV_8UC4:
//   {
//      QImage image( inMat.data,
//                    inMat.cols, inMat.rows,
//                    static_cast<int>(inMat.step),
//                    QImage::Format_ARGB32 );

//      return image;
//   }

//      // 8-bit, 3 channel
//   case CV_8UC3:
//   {
//      QImage image( inMat.data,
//                    inMat.cols, inMat.rows,
//                    static_cast<int>(inMat.step),
//                    QImage::Format_RGB888 );

//      return image.rgbSwapped();
//   }

//      // 8-bit, 1 channel
//   case CV_8UC1:
//   {

//      QImage image( inMat.data,
//                    inMat.cols, inMat.rows,
//                    static_cast<int>(inMat.step),
//                    QImage::Format_Grayscale8 );


//      return image;
//   }

//   default:
//      qWarning() << "cv::Mat image type not handled : " << inMat.type();
//      break;
//   }

//   return QImage();
//}

//QPixmap yolo_darknet::cvMatToQPixmap( const cv::Mat &inMat )
//{
//   return QPixmap::fromImage( cvMatToQImage( inMat ) );
//}

//cv::Mat yolo_darknet::QImageToCvMat( const QImage &inImage, bool inCloneImageData)
//{
//   switch ( inImage.format() )
//   {
//      // 8-bit, 4 channel
//      case QImage::Format_ARGB32:
//      case QImage::Format_ARGB32_Premultiplied:
//      {
//         cv::Mat  mat( inImage.height(), inImage.width(),
//                       CV_8UC4,
//                       const_cast<uchar*>(inImage.bits()),
//                       static_cast<size_t>(inImage.bytesPerLine())
//                       );

//         return (inCloneImageData ? mat.clone() : mat);
//      }

//      // 8-bit, 3 channel
//      case QImage::Format_RGB32:
//      {
//         if ( !inCloneImageData )
//         {
//            qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning so we don't modify the original QImage data";
//         }

//         cv::Mat  mat( inImage.height(), inImage.width(),
//                       CV_8UC4,
//                       const_cast<uchar*>(inImage.bits()),
//                       static_cast<size_t>(inImage.bytesPerLine())
//                       );

//         cv::Mat  matNoAlpha;

//         cv::cvtColor( mat, matNoAlpha, cv::COLOR_BGRA2BGR );   // drop the all-white alpha channel

//         return matNoAlpha;
//      }

//      // 8-bit, 3 channel
//      case QImage::Format_RGB888:
//      {
//         if ( !inCloneImageData )
//         {
//            qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning so we don't modify the original QImage data";
//         }

//         QImage   swapped = inImage.rgbSwapped();

//         return cv::Mat( swapped.height(), swapped.width(),
//                         CV_8UC3,
//                         const_cast<uchar*>(swapped.bits()),
//                         static_cast<size_t>(swapped.bytesPerLine())
//                         ).clone();
//      }

//      // 8-bit, 1 channel
//      case QImage::Format_Indexed8:
//      {
//         cv::Mat  mat( inImage.height(), inImage.width(),
//                       CV_8UC1,
//                       const_cast<uchar*>(inImage.bits()),
//                       static_cast<size_t>(inImage.bytesPerLine())
//                       );

//         return (inCloneImageData ? mat.clone() : mat);
//      }

//      default:
//         qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
//         break;
//   }

//   return cv::Mat();
//}

//// If inPixmap exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inPixmap's data
//// with the cv::Mat directly
////    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
//cv::Mat yolo_darknet::QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData)
//{
//   return QImageToCvMat( inPixmap.toImage(), inCloneImageData );
//}


