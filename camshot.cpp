#ifdef ANDROID

//#include <QCoreApplication>
#include <QImage>
#include <QDebug>
#include "camshot.h"


QImage CamShot::aBitmapToQImage(const QAndroidJniObject &aBitmap){
    if (!aBitmap.isValid())
        return QImage();
    jint aBitmapWidth = aBitmap.callMethod<jint>("getWidth");
    jint aBitmapHeight = aBitmap.callMethod<jint>("getHeight");
    QAndroidJniEnvironment env;
    const int32_t aBitmapPixelsCount = aBitmapWidth * aBitmapHeight;
    jintArray pixels = env->NewIntArray(aBitmapPixelsCount);
    jint aBitmapOffset = 0;
    jint aBitmapStride = aBitmapWidth;
    jint aBitmapX = 0;
    jint aBitmapY = 0;
    aBitmap.callMethod<void>("getPixels","([IIIIIII)V", pixels, aBitmapOffset, aBitmapStride, aBitmapX, aBitmapY, aBitmapWidth, aBitmapHeight);
    jint *pPixels = env->GetIntArrayElements(pixels, nullptr);
    QImage img(aBitmapWidth, aBitmapHeight, QImage::Format_ARGB32);
    int lineSzB = aBitmapWidth * sizeof(jint);
    for (int i = 0; i < aBitmapHeight; ++i){
        uchar *pDst = img.scanLine(i);
        const uchar *pSrc = reinterpret_cast<const uchar*>(pPixels + aBitmapWidth * i + aBitmapWidth);
        memcpy(pDst, pSrc, lineSzB);
    }
    env->DeleteLocalRef(pixels); //env->ReleaseIntArrayElements(pixels, pPixels, 0);
    return img;
}
QImage CamShot::camThumbnailToQImage(const QAndroidJniObject &data){

    QAndroidJniObject bundle = data.callObjectMethod("getExtras","()Landroid/os/Bundle;");
    qDebug()<<"bundle.isValid() "<<bundle.isValid()<<bundle.toString();

    QAndroidJniObject bundleKey = QAndroidJniObject::fromString("data");

    const QAndroidJniObject aBitmap (data.callObjectMethod("getParcelableExtra", "(Ljava/lang/String;)Landroid/os/Parcelable;", bundleKey.object<jstring>()));
    qDebug()<<"aBitmap.isValid() "<<aBitmap.isValid()<<aBitmap.toString();
    return aBitmapToQImage(aBitmap);
}
QImage CamShot::camImageToQImage(){
    QAndroidJniObject bitmap = QAndroidJniObject::callStaticObjectMethod("android/graphics/BitmapFactory","decodeFile","(Ljava/lang/String;)Landroid/graphics/Bitmap;",tempImgAbsPath.object<jobject>());
    qDebug()<<"bitmap.isValid() "<<bitmap.isValid()<<bitmap.toString();
    QImage img = aBitmapToQImage(bitmap);
//    if (tempImgFile.isValid())
//        tempImgFile.callMethod<jboolean>("delete");
    return img;
}
CamShot::ImgOrientation CamShot::needRotateAtRightAngle(){

    QAndroidJniObject exifInterface = QAndroidJniObject("android/media/ExifInterface","(Ljava/lang/String;)V",
                                                        tempImgAbsPath.object<jstring>());
    qDebug() << __FUNCTION__ << "exifInterface.isValid()=" << exifInterface.isValid();
    QAndroidJniObject TAG_ORIENTATION = QAndroidJniObject::getStaticObjectField<jstring>("android/media/ExifInterface", "TAG_ORIENTATION");
    qDebug() << __FUNCTION__ << "TAG_ORIENTATION.isValid()=" << TAG_ORIENTATION.isValid()<<TAG_ORIENTATION.toString();
    const jint orientation = exifInterface.callMethod<jint>("getAttributeInt","(Ljava/lang/String;I)I",TAG_ORIENTATION.object<jstring>(),static_cast<jint>(ORIENTATION_UNDEFINED));
    return static_cast<ImgOrientation>(orientation);
}

void CamShot::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data){
    Q_UNUSED(data);
    if ( receiverRequestCode == REQUEST_IMAGE_CAPTURE && resultCode == RESULT_OK )
    {
//        if (_thumbnailNotFullScaleRequested){
//            const QImage thumbnail (camThumbnailToQImage(data));
//            if (!thumbnail.isNull())
//                emit newShotCreated(thumbnail);
//            return;
//        }
        //        const ImgOrientation orientation = needRotateAtRightAngle();
        QImage image (camImageToQImage());
        if (!image.isNull()){
            // applyOrientation(image, orientation);
            // emit newShotCreated(image);
            emit newShotCreated(mSharedFolder,mImgFilleName);
        }
    }
}
void CamShot::aMakeShot(const bool &thumbnailNotFullScale) {
    QAndroidJniObject action = QAndroidJniObject::fromString("android.media.action.IMAGE_CAPTURE");

    QAndroidJniObject intent=QAndroidJniObject("android/content/Intent","(Ljava/lang/String;)V",
                                                 action.object<jstring>());
    qDebug() << __FUNCTION__ << "intent.isValid()=" << intent.isValid();
    _thumbnailNotFullScaleRequested = thumbnailNotFullScale;
    if (thumbnailNotFullScale) {

        QtAndroid::startActivity(intent, REQUEST_IMAGE_CAPTURE, this);
        return;
    }

    //    QAndroidJniObject context = QtAndroid::androidContext();
    QAndroidJniObject context = QNativeInterface::QAndroidApplication::context();
    QString contextStr (context.toString());
    qDebug() <<"Context: "<<contextStr;

    QAndroidJniObject extDir = context.callObjectMethod("getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;", NULL);
    qDebug() << __FUNCTION__ << "extDir.isValid()=" << extDir.isValid()<<extDir.toString();

    QAndroidJniObject extDirAbsPath = extDir.callObjectMethod("getAbsolutePath","()Ljava/lang/String;");

    extDirAbsPath = QAndroidJniObject::fromString(extDirAbsPath.toString() + "/shared");
    const QString extDirAbsPathStr = extDirAbsPath.toString();
    qDebug() << __FUNCTION__ << "extDirAbsPath.isValid()=" << extDirAbsPath.isValid()<<extDirAbsPathStr;

    QAndroidJniObject sharedFolder=QAndroidJniObject("java.io.File","(Ljava/lang/String;)V",
                                                       extDirAbsPath.object<jstring>());
    mSharedFolder = sharedFolder.toString();
    mImgFilleName = "_tmp.jpg";
    qDebug() << __FUNCTION__ << "sharedFolder.isValid()=" << sharedFolder.isValid()<<sharedFolder.toString();
    const jboolean sharedFolderCreated = sharedFolder.callMethod<jboolean>("mkdirs");
    Q_UNUSED(sharedFolderCreated);

    QAndroidJniObject suggestedFilePath = QAndroidJniObject::fromString(extDirAbsPathStr+"/"+"_tmp.jpg");
    qDebug() << __FUNCTION__ << "suggestedFilePath.isValid()=" << suggestedFilePath.isValid()<<suggestedFilePath.toString();

    QAndroidJniObject tempImgFile=QAndroidJniObject("java.io.File","(Ljava/lang/String;)V",
                                                      suggestedFilePath.object<jstring>());
    qDebug() << __FUNCTION__ << "fileExistsCheck.isValid()=" << tempImgFile.isValid()<<tempImgFile.toString();

    if (tempImgFile.isValid()){
        const jboolean deleted = tempImgFile.callMethod<jboolean>("delete");
        Q_UNUSED(deleted);
    }

    const jboolean fileCreated = tempImgFile.callMethod<jboolean>("createNewFile");
    Q_UNUSED(fileCreated);

    tempImgAbsPath = tempImgFile.callObjectMethod("getAbsolutePath","()Ljava/lang/String;");
    qDebug() << __FUNCTION__ << "tempImgAbsPath.isValid()=" << tempImgAbsPath.isValid()<<tempImgAbsPath.toString();

//    const QString contextFileProviderStr ("org.qtproject.example.qsketch.fileprovider");
    const QString contextFileProviderStr ("th.ac.ku.cpe.ske.rice_dtor.fileprovider");
    const char androidFileProvider  [] = "android/support/v4/content/FileProvider";
    //const char androidxFileProvider [] = "androidx/core/content/FileProvider";
    /*QAndroidJniObject*/ tempImgURI = QAndroidJniObject::callStaticObjectMethod(androidFileProvider, "getUriForFile", "(Landroid/content/Context;Ljava/lang/String;Ljava/io/File;)Landroid/net/Uri;",
                                                                                 context.object<jobject>(), QAndroidJniObject::fromString(contextFileProviderStr).object<jstring>(), tempImgFile.object<jobject>());
    qDebug() << __FUNCTION__ << "tempImgURI.isValid()=" << tempImgURI.isValid()<<tempImgURI.toString();

    QAndroidJniObject MediaStore__EXTRA_OUTPUT
        = QAndroidJniObject::getStaticObjectField("android/provider/MediaStore", "EXTRA_OUTPUT", "Ljava/lang/String;");
    qDebug() << "MediaStore__EXTRA_OUTPUT.isValid()=" << MediaStore__EXTRA_OUTPUT.isValid();

    intent.callObjectMethod("putExtra","(Ljava/lang/String;Landroid/os/Parcelable;)Landroid/content/Intent;",MediaStore__EXTRA_OUTPUT.object<jstring>(), tempImgURI.object<jobject>());
    qDebug() << __FUNCTION__ << "intent.isValid()=" << intent.isValid();
    QtAndroid::startActivity(intent, REQUEST_IMAGE_CAPTURE, this);
}

#endif
