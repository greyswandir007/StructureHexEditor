#ifndef JSONTREEMODEL_H
#define JSONTREEMODEL_H

#include "jsonstoreddata.h"

#include <QAbstractTableModel>

class JsonTreeModel : public QAbstractTableModel {
    Q_OBJECT
public:
    JsonTreeModel(QObject *parent = nullptr);

    void setRoot(JsonStoredData *value);

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;

    QVariant headerData(int section,Qt::Orientation orientation, int role=Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;

    void updateData();

private:
    JsonStoredData *root;
    QStringList headers;
};

#endif // JSONTREEMODEL_H
