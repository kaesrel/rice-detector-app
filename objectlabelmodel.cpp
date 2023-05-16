#include "objectlabelmodel.h"
#include <QBrush>
#include <QDebug>

ObjectLabelModel::ObjectLabelModel(QObject *parent)
    : QAbstractTableModel(parent),yolo(nullptr)
{
}

QVariant ObjectLabelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    static QStringList hTab{tr("class"),tr("score"),
                           tr("top"),tr("left"),
                           tr("bottom"),tr("right")};
    if(role==Qt::DisplayRole)
    {
       if(orientation==Qt::Horizontal)
       {
           if(section>=0 && section<hTab.size())
               return hTab[section];
       }
       else
       {
          return QString("%1").arg(section+1);
       }

    }
    else if(role==Qt::TextAlignmentRole)
    {
       if(orientation==Qt::Horizontal)
          return Qt::AlignHCenter;
       else
          return int(Qt::AlignRight|Qt::AlignVCenter);
    }
    return QVariant();
}

int ObjectLabelModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if(yolo==nullptr)
        return 0;
    return int(yolo->getResult().size());
}

int ObjectLabelModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant ObjectLabelModel::data(const QModelIndex &index, int role) const
{
    // FIXME: Implement me!
    int row=index.row();
    int col=index.column();
    if(yolo==nullptr)
        return QVariant();

    auto &curObj=yolo->getResult()[row];

    if(role==Qt::DisplayRole)
    {
        switch(col)
        {
        case 0:
            return yolo->getClassNames()[curObj.classId];
        case 1:
            return QString("%1").arg(curObj.confidence,0,'g',2);
        }
    }
    else if(role==Qt::BackgroundRole)
    {
        auto classColor=yolo->getClassColors()[curObj.classId];
        return QBrush(QColor(classColor[2],classColor[1],classColor[0]));
    }
    else if(role==Qt::TextAlignmentRole)
    {
        if(col==0)
            return Qt::AlignCenter;
        else
            return int(Qt::AlignRight|Qt::AlignVCenter);
    }
    return QVariant();
}

void ObjectLabelModel::createModel(const yolo_darknet *yolo)
{
    beginResetModel();
    this->yolo=yolo;
    endResetModel();
}
