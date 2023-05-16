#ifdef ANDROID

#include "qt5compat.h"

namespace QtAndroidCompat {

    QJniObject androidActivity() {
        return QJniObject(QtAndroidPrivate::activity());
    }

    void startActivity(const QJniObject &intent, int receiverRequestCode, QAndroidActivityResultReceiver *resultReceiver)
    {
        QtAndroidPrivate::startActivity(intent, receiverRequestCode, resultReceiver);
    }

    jint androidSdkVersion() {
        return QtAndroidPrivate::androidSdkVersion();
    }
}

#endif //ANDROID
