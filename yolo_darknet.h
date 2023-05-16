#ifndef YOLO_DARKNET_H
#define YOLO_DARKNET_H
#include <opencv2/dnn.hpp>
#include <QString>
#include <QPixmap>
#include <QString>
#include <QVector>

class yolo_darknet
{
public:
    class yolo_obj
    {
    public:
        yolo_obj(int classId,float confidence,cv::Rect box) :
            classId(classId),confidence(confidence),box(box)
        {}
        int classId;
        float confidence;
        cv::Rect box;
    };

    yolo_darknet();
    void loadConfig(const QString &classFile, const QString &configFile,const QString &weightFile,
                    float threshold,float nms);
    void process(const QPixmap &input);
    const QPixmap &getPixmap() const;
    const QStringList &getClassNames() const
    { return classNames; }
    const std::vector<cv::Scalar> &getClassColors() const
    { return classColors; }
    const std::vector<yolo_obj> &getResult() const
    { return objects; }
    float getConfThreshold() const
    { return threshold; }
    float getNMSThreshold() const
    { return nms; }

//    static QImage cvMatToQImage( const cv::Mat &inMat );
//    static QPixmap cvMatToQPixmap( const cv::Mat &inMat );
//    static cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true );
//    static cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData = true );

private:
    void drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame);
    int status;
    int netWidth,netHeight;
    float threshold,nms;
    cv::dnn::Net net;
    QStringList classNames;
    std::vector<cv::Scalar> classColors;
    std::vector<cv::String> outNames;
    std::vector<yolo_obj> objects;
    cv::Mat imgSrc,imgRes;
    mutable QPixmap pixRes;
};

#endif // YOLO_DARKNET_H
