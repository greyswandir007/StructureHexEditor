#ifndef JSONTREEMODEL_H
#define JSONTREEMODEL_H

#include <QAbstractTableModel>

class JsonTreeModel : public QAbstractTableModel {
    Q_OBJECT
public:
    JsonTreeModel(QObject *parent = nullptr);
};

#endif // JSONTREEMODEL_H
