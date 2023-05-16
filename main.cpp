#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
#ifdef ANDROID
    qputenv("QT_SCALE_FACTOR","0.8");
#endif

    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("ske");
    QCoreApplication::setOrganizationDomain("cpe.ku.ac.th");
    QCoreApplication::setApplicationName("Thai Rice Quality Detector");

//    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    QCoreApplication::setAttribute(Qt::AA_Use96Dpi);
//    QCoreApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
      const QString baseName = "rice_dtor_" + QLocale(locale).name();
      if (translator.load(":/i18n/" + baseName)) {
          a.installTranslator(&translator);
          break;
      }
    }

    MainWindow w;
    w.show();

    return a.exec();
}
