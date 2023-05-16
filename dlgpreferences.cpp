#include "dlgpreferences.h"
#include "ui_dlgpreferences.h"
#include <QSettings>
#include <QUrl>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

dlgPreferences::dlgPreferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgPreferences)
{
    ui->setupUi(this);
    QSettings settings;
//    ui->modelPath->setText(settings.value("YOLO/ModelPath").toString());
//    ui->classFilePath->setText(settings.value("YOLO/ClassFile").toString());
//    ui->configFilePath->setText(settings.value("YOLO/ConfigFile").toString());
//    ui->weightFilePath->setText(settings.value("YOLO/WeightFile").toString());

    ui->threshSpinBox->setValue(settings.value("YOLO/Threshold",0.4).toFloat());
    ui->nmsSpinBox->setValue(settings.value("YOLO/NMS",0.4).toFloat());
    ui->checkUndist->setChecked(settings.value("ImageView/Undistorted",false).toBool());
    ui->checkPers->setChecked(settings.value("ImageView/Perspective",false).toBool());
}

dlgPreferences::~dlgPreferences()
{
    delete ui;
}

//void dlgPreferences::on_pushButton_clicked()
//{
//    browse(ui->classFilePath, tr("names (*.names *.txt)"),
//           "YOLO/ClassFile");
//}

//void dlgPreferences::on_pushButton_2_clicked()
//{
//    browse(ui->configFilePath,tr("cfg (*.cfg)"),
//           "YOLO/ConfigFile");
//}

//void dlgPreferences::on_pushButton_3_clicked()
//{
//    browse(ui->weightFilePath, tr("weights (*.weights)")
//           ,"YOLO/WeightFile");
//}

//void dlgPreferences::browse(QLineEdit *lineEdit, const QString &filter,const QString &settingStr)
//{
//    QSettings settings;
//    QString path;
//    if(!lineEdit->text().isEmpty())
//        path=lineEdit->text();
//    else
//        path=settings.value(settingStr).toString();

//    QUrl url=QUrl::fromLocalFile(lineEdit->text());
//    path = QFileDialog::getOpenFileName(this, tr("Open File"),
//                                        path,
//                                        filter);
//    path=QDir::toNativeSeparators(path);
//    lineEdit->setText(path);
////    settings.setValue(str,path);
//}

void dlgPreferences::on_buttonBox_accepted()
{
    QSettings settings;

//    settings.setValue("YOLO/ClassFile",ui->classFilePath->text());
//    settings.setValue("YOLO/ConfigFile",ui->configFilePath->text());
//    settings.setValue("YOLO/WeightFile",ui->weightFilePath->text());
//    settings.setValue("YOLO/ModelPath",ui->modelPath->text());
    settings.setValue("YOLO/Threshold",ui->threshSpinBox->value());
    settings.setValue("YOLO/NMS",ui->nmsSpinBox->value());
    settings.setValue("ImageView/Undistorted",ui->checkUndist->isChecked());
    settings.setValue("ImageView/Perspective",ui->checkPers->isChecked());

//    qDebug() << __FUNCTION__;
//    qDebug() << settings.value("YOLO/ModelPath");
}

//void dlgPreferences::on_pushButton_clicked()
//{
//    QSettings settings;
//    QString path;
//    if(!ui->modelPath->text().isEmpty())
//        path=ui->modelPath->text();
//    else
//        path=settings.value("YOLO/ModelPath").toString();

//    QUrl url=QUrl::fromLocalFile(ui->modelPath->text());

//    path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
//                                             path,
//                                             QFileDialog::ShowDirsOnly
//                                             | QFileDialog::DontResolveSymlinks);
//    path=QDir::toNativeSeparators(path);
//    ui->modelPath->setText(path);
//    QDir dir(path);

//    if(!dir.isReadable())
//    {
//        QMessageBox::warning(this,"Warning",tr("Folder is unable to read."));
//        return;
//    }
//}

