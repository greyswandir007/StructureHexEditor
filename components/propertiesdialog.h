#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include "propertiesmodel.h"
#include "propertydelegate.h"

#include <QDialog>
#include <QGraphicsItem>

namespace Ui {
class PropertiesDialog;
}

class PropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PropertiesDialog(QWidget *parent = nullptr);
    ~PropertiesDialog();

    void setItem(QString policy, int fixedCount);
    QString getPolicy();
    int getFixedCount();

private:
    Ui::PropertiesDialog *ui;

    PropertiesModel *propertyModel;
    QGraphicsItem *mainItem;
    QMap<QString, QVariant> properties;
    QStringList itemPropertyNames;
    QList<int> itemPropertyIndexes;
    QList<bool> itemPropertyMultiply;
    PropertyDelegate *propertyDelegate;
};

#endif // PROPERTIESDIALOG_H
