#ifdef ANDROID

#ifndef CAMSHOT_H
#define CAMSHOT_H

#include <QObject>
#include "qt5compat.h"
#include <QImage>
#include <QString>
#include <QDebug>

class CamShot : public QObject, public QAndroidActivityResultReceiver
{
    Q_OBJECT
public:
    static const int RESULT_OK = -1;
    static const int REQUEST_IMAGE_CAPTURE = 1;
    static const int REQUEST_TAKE_PHOTO = REQUEST_IMAGE_CAPTURE;
    enum ImgOrientation {ORIENTATION_UNDEFINED = 0, ORIENTATION_NORMAL = 1, ORIENTATION_FLIP_HORIZONTAL = 2, ORIENTATION_ROTATE_180 = 3, ORIENTATION_FLIP_VERTICAL = 4, ORIENTATION_TRANSPOSE = 5,
                          ORIENTATION_ROTATE_90 = 6, ORIENTATION_TRANSVERSE = 7, ORIENTATION_ROTATE_270 = 8};
    void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data) override;
    static QImage aBitmapToQImage(const QAndroidJniObject &aBitmap);
    static QImage camThumbnailToQImage(const QAndroidJniObject &data);
    ImgOrientation needRotateAtRightAngle();
    QImage camImageToQImage();
    static void applyOrientation(QImage &img, const ImgOrientation &orientation);


    explicit CamShot(QObject *parent = nullptr):QObject(parent),QAndroidActivityResultReceiver(){}
    ~CamShot() {};    
private:
    QAndroidJniObject tempImgURI;
    QAndroidJniObject tempImgFile;
    QAndroidJniObject tempImgAbsPath;
    bool _thumbnailNotFullScaleRequested;
    QString mSharedFolder,mImgFilleName;
public slots:
    void aMakeShot(const bool &thumbnailNotFullScale = false);
signals:
    void newShotCreated(const QImage &img);
    void newShotCreated(const QString &path, const QString &file);
};

#endif // CAMSHOT_H
#endif // ANDROID
