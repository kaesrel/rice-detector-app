#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGlobal>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QString>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths>
#include <QScreen>
#include <QFutureWatcher>
#include <QFuture>
#include <QIcon>
#include <QtConcurrent>
#include <QListView>
#include <QList>
#if QT_VERSION >= QT_VERSION_CHECK(6,5,0)
#include <QPermissions>
#endif
#include "dlgpreferences.h"
#include "objectlabelmodel.h"
#include "warper.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),m_scene(nullptr),
    progressDialog(nullptr),calWizard(nullptr)
{
    ui->setupUi(this);
#ifdef ANDROID
    ui->pushButton->setText(tr("Photo"));
    ui->statusBar->hide();
    ui->menuBar->show();
//    ui->menuBar->activateWindow();
#endif
    updateComboStep();
    calib.loadCalibData();

//    ui->listFilename->setModel(&m_fmodel);
    ui->resTB->setModel(&m_objModel);
    ui->resTB->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

//    connect(ui->listFilename->selectionModel(),
//            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
//                  this, SLOT(fileSelectionChanged(QItemSelection)));
    loadYolo();
    connect(&watcher, SIGNAL(finished()), this, SLOT(UpdateImagesView()));
#ifdef ANDROID
//    connect(&cam, SIGNAL(newShotCreated(QImage)),
//            this, SLOT(showImage(QImage)));
    connect(&cam, SIGNAL(newShotCreated(QString,QString)),
            this, SLOT(showImage(QString,QString)));
#endif
//    connect(&watcher, &QFutureWatcher<void>::finished, &progressDialog, &QProgressDialog::reset);
//    connect(&watcher,  &QFutureWatcher<void>::progressRangeChanged, &progressDialog, &QProgressDialog::setRange);
//    connect(&watcher, &QFutureWatcher<void>::progressValueChanged,  &progressDialog, &QProgressDialog::setValue);
//    connect(&progressDialog, &QProgressDialog::canceled, &watcher, &QFutureWatcher<void>::cancel);

//    showImage("D:/work/bert/rice_dat/dense_and_sparse/rice_5_dense","IMG_20230115_162458.jpg");
}

void MainWindow::updateComboStep()
{
    QSettings settings;
    bool showUndist = settings.value("ImageView/Undistorted",false).toBool();
    bool showPers = settings.value("ImageView/Perspective",false).toBool();

    if(!showUndist && ui->comboStep->currentIndex()==1)
        ui->comboStep->setCurrentIndex(2);
    if(!showPers && ui->comboStep->currentIndex()==2)
        ui->comboStep->setCurrentIndex(3);

    qobject_cast<QListView *>(ui->comboStep->view())->setRowHidden(1, !showUndist);
    qobject_cast<QListView *>(ui->comboStep->view())->setRowHidden(2, !showPers);
}

void MainWindow::createModelPath(QString &classFilePath,QString &configFilePath,QString &weightFilePath)
{
    QString prefix;
    QStringList fileNames = {"obj.names", "yolov4_rice.cfg", "yolov4_rice_best.weights"};
#ifdef Q_OS_ANDROID
    QString DataStoragePath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppDataLocation);
    prefix = DataStoragePath+'/';
    for(int i=0; i<fileNames.size(); i++)
    {
        QFile file;
        file.setFileName(QString("assets:/model_data/")+fileNames[i]);
        QString destFileName = prefix + fileNames[i];
        if (file.exists(destFileName))
            continue;
        file.copy(destFileName);
        QFile::setPermissions(destFileName, QFile::WriteOwner | QFile::ReadOwner);
    }
#else
    prefix = QCoreApplication::applicationDirPath() + "/model_data/";
#endif
    classFilePath = QDir(prefix+fileNames[0]).path();
    configFilePath = QDir(prefix+fileNames[1]).path();
    weightFilePath = QDir(prefix+fileNames[2]).path();
    return;
}

void MainWindow::loadYolo()
{
    QSettings settings;
    QString classFilePath,configFilePath,weightFilePath;

    float threshold,nms;

//    classFilePath=settings.value("YOLO/ClassFile").toString();
//    configFilePath=settings.value("YOLO/ConfigFile").toString();
//    weightFilePath=settings.value("YOLO/WeightFile").toString();
//    modelPath=settings.value("YOLO/ModelPath").toString();
//    modelPath="assets:/model_data";
    createModelPath(classFilePath,configFilePath,weightFilePath);

//    classFilePath=modelPath+"/obj.names";
//    configFilePath=modelPath+"/yolov4_rice.cfg";
//    weightFilePath=modelPath+"/yolov4_rice_best.weights";

    threshold=settings.value("YOLO/Threshold",0.4).toFloat();
    nms=settings.value("YOLO/NMS",0.4).toFloat();

    qDebug() << "threshold="<< threshold << "nms=" << nms;
    qDebug() << classFilePath << configFilePath << weightFilePath;

    if(classFilePath.size()!=0 && configFilePath.size()!=0 && weightFilePath.size()!=0)
        yolo.loadConfig(classFilePath,configFilePath,
                        weightFilePath,threshold,nms);

//    QModelIndex idx(ui->listFilename->currentIndex());
//    if(idx.isValid())
//    {
//        QItemSelection selection(idx,idx);
//        fileSelectionChanged(selection);
//    }

    return;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
#ifdef ANDROID

#if QT_VERSION >= QT_VERSION_CHECK(6,5,0)

    QList<QPermission> permissions = {
        QCameraPermission{}
    };

    for (int i=0; i < permissions.size(); i++)
    {
        QPermission &perm = permissions[i];

        switch (qApp->checkPermission(perm)) {
        case Qt::PermissionStatus::Undetermined:
            qApp->requestPermission(perm, [](const QPermission &permission) {
                Q_UNUSED(permission);
            });
            i--;
            break;
        case Qt::PermissionStatus::Denied:
            return;
        case Qt::PermissionStatus::Granted:
            break; // Proceed
        }
    }

#else

    QList<QtAndroidPrivate::PermissionType> permissions = {
        QtAndroidPrivate::Storage,
        QtAndroidPrivate::Camera
    };

    for (const auto &perm : permissions)
    {
        auto r = QtAndroidPrivate::checkPermission(perm).result();
        if (r == QtAndroidPrivate::Denied)
        {
            r = QtAndroidPrivate::requestPermission(perm).result();
                if (r == QtAndroidPrivate::Denied)
                    return;
        }
    }


#endif

    cam.aMakeShot();

#else

//    QSettings settings;
//    QString path;
//    if(!ui->ImagePath->text().isEmpty())
//        path=ui->ImagePath->text();
//    else
//        path=settings.value("MainWindow/ImagePath").toString();

//    QUrl url=QUrl::fromLocalFile(ui->ImagePath->text());

//    path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
//                                             path,
//                                             QFileDialog::ShowDirsOnly
//                                             | QFileDialog::DontResolveSymlinks);
//    path=QDir::toNativeSeparators(path);
//    ui->ImagePath->setText(path);
//    settings.setValue("MainWindow/ImagePath",path);

//    QDir dir(path);

//    if(!dir.isReadable())
//    {
//        QMessageBox::warning(this,"Warning",tr("Folder is unable to read."));
//        return;
//    }

//    //    pathList = dir.entryList(filterList,QDir::Files);
//    loadPath();
#endif
}

void MainWindow::loadPath()
{
//   QDir dir(ui->ImagePath->text());
//   QStringList filters;
//   filters << "*.png" << "*.PNG" << "*.jpg" << "*.JPG"
//           << "*.gif" << "*.GIF" << "*.tif" << "*.TIFF";
//   QStringList images;

//   delete m_scene;
//   m_scene=nullptr;
//   if(!dir.isReadable())
//      QMessageBox::warning(this,"Warning",tr("Folder is unable to read."));
//   else
//      images = dir.entryList(filters,QDir::Files);

//   //ui->comboFilenames->addItems(images);
////   m_fmodel.setStringList(images);
////   ui->listFilename->setCurrentIndex(QModelIndex());
////   ui->listFilename->setCurrentIndex(m_fmodel.index(0,0));
}

void MainWindow::showImage(const QString &fpath,const QString &fname)
{
    QString filename=fpath+'/'+fname;

    QImage image;
    if(!image.load(filename))

//    QPixmap tmppx;
//    if(!tmppx.load(filename))
    {
        ui->statusBar->showMessage(tr("Unable to open this file."),2000);
        return;
    }
//    showImage(tmppx.toImage());
    showImage(image);
}

void MainWindow::showImage(const QImage &img)
{
    if(m_scene!=nullptr)
        delete m_scene;
    m_pixmaps.clear();
    m_pixmaps.resize(4);
    //   m_pixmaps.resize(m_imgProcessor.getProcNames().size()+1);

    m_scene=new QGraphicsScene(this);

    QImage img_(img);
    scaleImageDown(img_);
    if(img_.width() < img_.height())
    {
        // errid=1;
        // return false;

        QTransform tmat;
        tmat.rotate(-90.0);
        img_ = img_.transformed(tmat);
    }

    QPixmap tmppx;
    tmppx.convertFromImage(img_);
    //   tmppx=tmppx.scaledToHeight(1024);
    m_pixmaps[0]=m_scene->addPixmap(tmppx);

    runRecognition();
}

//void MainWindow::fileSelectionChanged(const QItemSelection& selection)
//{
//    showImage(ui->ImagePath->text(),selection.indexes().first().data().toString());
//}

void MainWindow::on_checkFitIn_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    if(m_scene && m_pixmaps.size()>0 && m_pixmaps[0])
    {
       if(ui->checkFitIn->isChecked())
       {
          ui->graphicsView->fitInView(m_pixmaps[0],Qt::KeepAspectRatio);
          ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
          ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
       }
       else
       {
          ui->graphicsView->fitInView(ui->graphicsView->rect());
//          ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//          ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
       }
    }
}

void MainWindow::switchImagesView()
{
   if(m_scene==nullptr)
      return;
   int index=ui->comboStep->currentIndex();

   for(int i=0;i<m_pixmaps.size();i++)
   {
      if(m_pixmaps[i]==nullptr)
      {
          continue;
      }
      if(i==index)
         m_pixmaps[i]->setVisible(true);
      else
         m_pixmaps[i]->setVisible(false);
   }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    on_checkFitIn_stateChanged(0);
}

void MainWindow::on_comboStep_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    qDebug() << "combobox index is " << index;
    switchImagesView();
}

void MainWindow::on_actionAbout_Thai_Rice_Quality_Detector_triggered()
{
    QMessageBox::about(this,"About this program",
    "<p align='center'><h4>Thai Quality Defect Detector</h4>by<br/><br/>Me<br/>and<br/>friend</p>"
    "<p>This program based on YOLO object detection model provided by Joseph Redmond."
    "Please see <a href='https://pjreddie.com/darknet/yolo'>pjreddie.com/darknet/yolo</a> for more detail."
    "</p>");
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this,"About Qt");
}

void MainWindow::on_action_Preferences_triggered()
{
    dlgPreferences *dlg=new dlgPreferences(this);
    int result;
//    QPalette palette = this->palette();
//    QPalette darkPalette = this->palette();
//    darkPalette.setColor(QPalette::Window,QColor("black"));
//    this->setPalette(darkPalette);
    dlg->setModal(true);
    result=dlg->exec();
//    this->setPalette(palette);
    if(result==QDialog::Accepted)
    {
        QSettings settings;
        float thresh = settings.value("YOLO/NMS",0.4).toFloat();
        float nms = settings.value("YOLO/Threshold",0.4).toFloat();
        if( nms != yolo.getNMSThreshold() || thresh != yolo.getConfThreshold())
        {
            loadYolo();
            runRecognition(true);
        }
        updateComboStep();
    }
}

void MainWindow::on_actionE_xit_triggered()
{
    this->close();
}


void MainWindow::runRecognition(bool skipUnDistorted)
{
    if (m_pixmaps[0] == nullptr)
        return;
    if(progressDialog == nullptr)
    {
        progressDialog = new QProgressDialog(this);
        progressDialog->setLabelText(tr("Recognition process is running."));
    }

    progressDialog->disconnect();
    watcher.disconnect(progressDialog);

    // connect object to the finished signal.
    connect(&watcher, &QFutureWatcher<void>::finished, progressDialog, &QProgressDialog::reset);
    connect(&watcher,  &QFutureWatcher<void>::progressRangeChanged, progressDialog, &QProgressDialog::setRange);
    connect(&watcher, &QFutureWatcher<void>::progressValueChanged,  progressDialog, &QProgressDialog::setValue);
    connect(progressDialog, &QProgressDialog::canceled, &watcher, &QFutureWatcher<void>::cancel);

    // Start the computation.
    //   QFuture<void> future = QtConcurrent::run(yolo, &yolo_darknet::process, tmppx);

//    for(int i=1; i < m_pixmaps.size(); i++)
//    {
//        qDebug() << i << ":" << m_pixmaps[i];
//        if(m_pixmaps[i] != nullptr)
//        {
//            m_scene->removeItem(m_pixmaps[i]);
//            delete m_pixmaps[i];
//        }
//    }

    QFuture<void> future = QtConcurrent::run([=] () {
        QGraphicsPixmapItem *pInputPix = m_pixmaps[0];
        QPixmap pix = pInputPix->pixmap();

        if(skipUnDistorted == false)
        {
            if(calib.isValid()) // && m_pixmaps[0] != nullptr)
            {
                QImage img = pix.toImage();
                calib.unDistorted(img);
                pix.convertFromImage(img);
                if(m_pixmaps[1] != nullptr)
                {
                    m_scene->removeItem(m_pixmaps[1]);
                    delete m_pixmaps[1];
                }
                m_pixmaps[1] = m_scene->addPixmap(pix);
                qDebug() << "calib add to m_pixmaps[1]: " << m_pixmaps[1];
                qDebug() << "m_scene.items().count(): " << m_scene->items().count();
            }
            Warper warper;
            bool isWarpOk = warper.warp(pix);
            if(isWarpOk)
            {
                pix = warper.getQPixmap();
                if(m_pixmaps[2] != nullptr)
                {
                    m_scene->removeItem(m_pixmaps[2]);
                    delete m_pixmaps[2];
                }
                m_pixmaps[2] = m_scene->addPixmap(pix);
            }

        }
        yolo.process(pix);
//        m_pixmaps[3] = m_scene->addPixmap(yolo.getPixmap());
    });
//    if(future.isFinished())
//    {
//        emit watcher.finished();
//        return;
//    }

    watcher.setFuture(future);

    progressDialog->exec();

    watcher.waitForFinished();
    watcher.disconnect(progressDialog);
    progressDialog->disconnect();
}

void MainWindow::UpdateImagesView()
{
    if(m_pixmaps[3] != nullptr)
    {
        m_scene->removeItem(m_pixmaps[3]);
        delete m_pixmaps[3];
    }
    m_pixmaps[3] = m_scene->addPixmap(yolo.getPixmap());
    switchImagesView();
    ui->graphicsView->setScene(m_scene);
    ui->graphicsView->show();
    qDebug() << "m_scene.items().count(): " << m_scene->items().count();
    on_checkFitIn_stateChanged(0);

    qDebug() << __FUNCTION__ << yolo.getResult().size();
    m_objModel.createModel(&yolo);
    ui->resTB->setModel(nullptr);
    ui->resTB->setModel(&m_objModel);
}

void MainWindow::on_actionCamera_Calibration_triggered()
{
    if(calWizard != nullptr)
    {
        disconnect(calWizard);
        delete calWizard;
    }
    calWizard = new CalibrationWizard(this);
    connect(calWizard,SIGNAL(newCalibrateFinished()),
            this, SLOT(UpdateCalibrator()));

    calWizard->setWindowModality(Qt::ApplicationModal);
    calWizard->show();
    qDebug() << "Calibration changed: " << calWizard->isFinished;
//    if(calWizard->isFinished)
//    {
//        calib.loadCalibData();
//        runRecognition();
//    }
}

void MainWindow::UpdateCalibrator()
{
    calib.loadCalibData();
    runRecognition();
}
