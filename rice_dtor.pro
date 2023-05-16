
QT       += core gui core-private concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rice_dtor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        calibrationwizard.cpp \
        calibrator.cpp \
        camshot.cpp \
        dlgpreferences.cpp \
        graphicsview.cpp \
        img_utils.cpp \
        main.cpp \
        mainwindow.cpp \
        objectlabelmodel.cpp \
        qt5compat.cpp \
        warper.cpp \
        yolo_darknet.cpp

HEADERS += \
        calibrationwizard.h \
        calibrator.h \
        camshot.h \
        dlgpreferences.h \
        graphicsview.h \
        img_utils.h \
        mainwindow.h \
        objectlabelmodel.h \
        qt5compat.h \
        warper.h \
        yolo_darknet.h

FORMS += \
        calibrationwizard.ui \
        dlgpreferences.ui \
        mainwindow.ui


TRANSLATIONS += \
    rice_dtor_th_TH.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

folder_01.source = model_data
folder_01.target = model_data
DEPLOYMENTFOLDERS += folder_01

unix:!android {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv4
}

win32 {
    OPENCV_WIN32_DIR = C:\opt\opencv-4.7.0\build\windows\install

    INCLUDEPATH += $${OPENCV_WIN32_DIR}/include
    #INCLUDEPATH += C:\opt\opencv-4.7.0\build\windows\install\include

    #LIBS += -LC:\opt\opencv-4.7.0\build\windows\install\x64\mingw\bin \
    LIBS += -L$${OPENCV_WIN32_DIR}/x64/mingw/bin \
            -lopencv_core470        \
            -lopencv_imgcodecs470   \
            -lopencv_imgproc470     \
            -lopencv_calib3d470     \
            -lopencv_dnn470

    #-lopencv_highgui470     \
    #-lopencv_imgcodecs470   \
    #-lopencv_imgproc470     \
    #-lopencv_features2d470  \
}

android {
    OPENCV_ANDROID_DIR=C:\opt\opencv-4.7.0\build\android\install

#    contains(ANDROID_TARGET_ARCH, armeabi-v7a) {
#        isEmpty(OPENCV_ANDROID_DIR) {
#        error("Let OPENCV_ANDROID point to the opencv-android-sdk, recommended: v4.x")
#        }
        INCLUDEPATH += $${OPENCV_ANDROID_DIR}/sdk/native/jni/include
#        OPENCV3RDPARTYLIBS = $${OPENCV_ANDROID_DIR}/sdk/native/3rdparty/libs/armeabi-v7a
#        OPENCVNATIVELIBS = $${OPENCV_ANDROID_DIR}/sdk/native/libs/armeabi-v7a
        OPENCVNATIVELIBS = $${OPENCV_ANDROID_DIR}/sdk/native/libs/arm64-v8a

    ANDROID_EXTRA_LIBS = \
        $${OPENCVNATIVELIBS}/libopencv_core.so \
        $${OPENCVNATIVELIBS}/libopencv_imgcodecs.so \
        $${OPENCVNATIVELIBS}/libopencv_imgproc.so \
        $${OPENCVNATIVELIBS}/libopencv_calib3d.so \
        $${OPENCVNATIVELIBS}/libopencv_features2d.so \
        $${OPENCVNATIVELIBS}/libopencv_flann.so \
        $${OPENCVNATIVELIBS}/libopencv_dnn.so

    LIBS += -L$${OPENCVNATIVELIBS} \
            -lopencv_core        \
            -lopencv_imgcodecs   \
            -lopencv_imgproc     \
            -lopencv_calib3d     \
            -lopencv_features2d  \
            -lopencv_flann       \
            -lopencv_dnn

#        LIBS += $$OPENCVNATIVELIBS/libopencv_calib3d.so \
#        $$OPENCVNATIVELIBS/libopencv_core.so \
#        $$OPENCVNATIVELIBS/libopencv_dnn.so \
#        $$OPENCVNATIVELIBS/libopencv_imgcodecs.so \
#        $$OPENCVNATIVELIBS/libopencv_imgproc.so

#        $$OPENCVNATIVELIBS/libopencv_java4.so \
#        $$OPENCVNATIVELIBS/libopencv_features2d.so \
#        $$OPENCVNATIVELIBS/libopencv_flann.so \
#        $$OPENCVNATIVELIBS/libopencv_gapi.so \
#        $$OPENCVNATIVELIBS/libopencv_highgui.so \
#        $$OPENCVNATIVELIBS/libopencv_ml.so \
#        $$OPENCVNATIVELIBS/libopencv_objdetect.so \
#        $$OPENCVNATIVELIBS/libopencv_photo.so \
#        $$OPENCVNATIVELIBS/libopencv_stitching.so \
#        $$OPENCVNATIVELIBS/libopencv_video.so \
#        $$OPENCVNATIVELIBS/libopencv_videoio.so

#    } else {
#        error(“Unsupported architecture: $$ANDROID_TARGET_ARCH”)
#    }

    assets.files = \
        model_data/obj.names \
        model_data/yolov4_rice.cfg \
        model_data/yolov4_rice_best.weights
    assets.path = /assets/model_data
    INSTALLS += assets
}


contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml \
    android/res/xml/file_paths.xml
