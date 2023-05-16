#ifdef ANDROID

#ifndef QT5COMPAT_H
#define QT5COMPAT_H

#include <QJniObject>
#include <QJniEnvironment>

#include <QtCore/private/qandroidextras_p.h>


typedef QJniObject QAndroidJniObject;
typedef QJniEnvironment QAndroidJniEnvironment;

namespace QtAndroidCompat {
using namespace QtAndroidPrivate;

PermissionResult checkPermission(const QString &permissison);

QJniObject androidActivity();

void startActivity(const QJniObject &intent, int receiverRequestCode, QAndroidActivityResultReceiver *resultReceiver);

jint androidSdkVersion();
}

#define QtAndroid QtAndroidCompat

#endif // QT5COMPAT_H
#endif // ANDROID


