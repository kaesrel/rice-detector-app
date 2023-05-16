#ifndef CALIBRATIONWIZARD_H
#define CALIBRATIONWIZARD_H

#include <QMainWindow>
#include <QStringList>
#include <QCloseEvent>
#include "calibrator.h"
#include "camshot.h"

namespace Ui {
class CalibrationWizard;
}

class CalibrationWizard : public QMainWindow
{
    Q_OBJECT

public:
    explicit CalibrationWizard(QWidget *parent = nullptr);
    ~CalibrationWizard();
    bool isFinished = false;

private slots:
    void addImage(const QString &fpath,const QString &fname);

    void on_cmdQuit_0_clicked();

    void on_cmdContinue_0_clicked();

    void on_cmdQuit_1_clicked();

    void on_cmdContinue_1_clicked();

    void on_cmdQuit_2_clicked();

    void on_cmdPhoto_clicked();

    void on_cmdFinish_clicked();

private:
//    bool event(QEvent *event) override;

    void closeEvent(QCloseEvent *event) override;
    void updateLabelShot();

    Ui::CalibrationWizard *ui;
    CamShot cam;
    Calibrator calib;
    QStringList errMesgList = QStringList {
        "",
        tr("Picture orientation is not landscape. Please use camera in landscape mode."),
        tr("No checkerboard can be found in the picture. This may due to poor lighting.")
    };
    const int NUMBER_REQUIRED = 4;

signals:
    void newCalibrateFinished();
};

#endif // CALIBRATIONWIZARD_H
