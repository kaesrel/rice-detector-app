#include "calibrationwizard.h"
#include "ui_calibrationwizard.h"
#include <QShowEvent>
#include <QDebug>
#include <QMessageBox>

CalibrationWizard::CalibrationWizard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CalibrationWizard)
{
    ui->setupUi(this);
    calib.loadCalibData();
    if(calib.isValid())
        ui->stackedWidget->setCurrentIndex(0);
    else
        ui->stackedWidget->setCurrentIndex(1);
//    ui->stackedWidget->setCurrentIndex(0);
}

CalibrationWizard::~CalibrationWizard()
{
    delete ui;
}

void CalibrationWizard::closeEvent(QCloseEvent *event)
{
    if (isFinished ||
        QMessageBox::Yes == QMessageBox::question(this, tr("Close Confirmation"), tr("Exit?"), QMessageBox::Yes | QMessageBox::No))
    {
        if (isFinished)
            emit newCalibrateFinished();
        event->accept();
    }
    else
        event->ignore();
}

//bool CalibrationWizard::event(QEvent *event)
//{
//    if(event->type() == QEvent::Show)
//    {
//        qDebug() << "CalibrationWizard is shown.";

//    }
//    return QMainWindow::event(event);
//}


void CalibrationWizard::on_cmdQuit_0_clicked()
{
    close();
}

void CalibrationWizard::on_cmdContinue_0_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void CalibrationWizard::on_cmdQuit_1_clicked()
{
    close();
}

void CalibrationWizard::on_cmdContinue_1_clicked()
{
    calib.calibrateInit();
    connect(&cam, SIGNAL(newShotCreated(QString,QString)),
            this, SLOT(addImage(QString,QString)));
    ui->stackedWidget->setCurrentIndex(2);
    updateLabelShot();
}

void CalibrationWizard::on_cmdQuit_2_clicked()
{
    close();
}

void CalibrationWizard::on_cmdPhoto_clicked()
{
    cam.aMakeShot();
}

void CalibrationWizard::on_cmdFinish_clicked()
{
    close();
}

void CalibrationWizard::addImage(const QString &fpath,const QString &fname)
{
    QString filename=fpath+'/'+fname;

    QImage image;
    if(!image.load(filename))
    {
//        ui->statusBar->showMessage(tr("Unable to open this file."),2000);
        return;
    }
    calib.calibrateAddImage(image);
    int errid=calib.getErrId();
    if(errid != 0)
    {
        QMessageBox::warning(this,tr("Invalid Photo"),errMesgList.at(errid));
        return;
    }
    if(calib.getPictureCount()==NUMBER_REQUIRED)
    {
        calib.calibrateFinished();
        calib.saveCalibData();
        isFinished = true;
        ui->stackedWidget->setCurrentIndex(3);
    }

    updateLabelShot();
}

void CalibrationWizard::updateLabelShot()
{
    QString s=QString(tr("Click Photo button to take photo no. %1 of %2."))
                    .arg(calib.getPictureCount()+1)
                    .arg(NUMBER_REQUIRED);
    ui->labelShot->setText(s);
}



