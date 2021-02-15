#include "propertiesmodel.h"

PropertiesModel::PropertiesModel(QObject *parent) :
    QAbstractTableModel(parent) {
    headers << tr("Property") << tr("Value");
}

void PropertiesModel::setValueMap(QMap<QString, QVariant> values) {
    mainValueMap.clear();
    for (QString key : values.keys()) {
        mainValueMap.insert(key, values[key]);
    }
}

QMap<QString, QVariant> PropertiesModel::valueMap() {
    return mainValueMap;
}

QVariant PropertiesModel::data(const QModelIndex &index, int role) const {
    if (index.isValid() && index.row() < mainValueMap.keys().size() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        if (index.column() == 0) {
            return mainValueMap.keys().at(index.row());
        } else if (index.column() == 1) {
            return mainValueMap[mainValueMap.keys().at(index.row())];
        }
    }
    return QVariant();
}

bool PropertiesModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid() && index.row() < mainValueMap.keys().size() && role == Qt::EditRole && index.column() == 1) {
        mainValueMap[mainValueMap.keys().at(index.row())] = value;
        return true;
    }
    return false;
}

int PropertiesModel::rowCount(const QModelIndex &parent) const {
    return parent == QModelIndex() ? mainValueMap.keys().size() : 0;
}

int PropertiesModel::columnCount(const QModelIndex &parent) const {
    return parent == QModelIndex() ? headers.count() : 0;
}

QVariant PropertiesModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    return orientation == Qt::Horizontal ? headers.at(section) : QString("%1").arg(section + 1);
}

Qt::ItemFlags PropertiesModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }
    return index.column() == 1 ? QAbstractItemModel::flags(index) | Qt::ItemIsEditable : Qt::ItemIsEnabled;
}
