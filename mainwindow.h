#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QVector>
#include <QItemSelection>
#include <QFutureWatcher>
#include <QProgressDialog>
#include "calibrationwizard.h"
#include "yolo_darknet.h"
#include "objectlabelmodel.h"
#include "camshot.h"
#include "calibrator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void loadPath();
    void switchImagesView();
    void resizeEvent(QResizeEvent *event);
    void createModelPath(QString &classFilePath,QString &configFilePath,QString &weightFilePath);
    void loadYolo();


private slots:
    void showImage(const QImage &img);
    void showImage(const QString &fpath,const QString &fname);

    void on_pushButton_clicked();

    void UpdateImagesView();
    void UpdateCalibrator();
//    void fileSelectionChanged(const QItemSelection& selection);

    void on_checkFitIn_stateChanged(int arg1);

    void on_comboStep_currentIndexChanged(int index);

    void on_actionAbout_Qt_triggered();

    void on_action_Preferences_triggered();

    void on_actionE_xit_triggered();

    void on_actionAbout_Thai_Rice_Quality_Detector_triggered();

    void on_actionCamera_Calibration_triggered();

private:
//    void modelChanged();
    void updateComboStep();
    void runRecognition(bool skipUnDistorted=false);
    Ui::MainWindow *ui;
    QStringListModel m_fmodel;
    QGraphicsScene *m_scene;
    QVector<QGraphicsPixmapItem*> m_pixmaps;
    yolo_darknet yolo;
    ObjectLabelModel m_objModel;
    QFutureWatcher<void> watcher;
    QProgressDialog *progressDialog;
    Calibrator calib;
    CalibrationWizard *calWizard;
#ifdef ANDROID
    CamShot cam;
#endif
};

#endif // MAINWINDOW_H
