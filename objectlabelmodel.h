#ifndef OBJECTLABELMODEL_H
#define OBJECTLABELMODEL_H

#include <QAbstractTableModel>
#include "yolo_darknet.h"

class ObjectLabelModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ObjectLabelModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void createModel(const yolo_darknet *yolo);
private:
    const yolo_darknet *yolo;
};

#endif // OBJECTLABELMODEL_H
