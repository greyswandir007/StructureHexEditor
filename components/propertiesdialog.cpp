#include "propertiesdialog.h"
#include "ui_propertiesdialog.h"

PropertiesDialog::PropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PropertiesDialog) {
    ui->setupUi(this);
    propertyModel = new PropertiesModel(nullptr);
    ui->propertiesTable->setModel(propertyModel);
    mainItem = nullptr;
    itemPropertyNames << tr("Column policy") << tr("Fixed colomn count");
    propertyDelegate = new PropertyDelegate();
    ui->propertiesTable->setItemDelegate(propertyDelegate);
}

PropertiesDialog::~PropertiesDialog() {
    delete ui;
    delete propertyDelegate;
}

void PropertiesDialog::setItem(QString policy, int fixedCount) {
    properties.insert(itemPropertyNames.at(0), policy);
    properties.insert(itemPropertyNames.at(1), fixedCount);
    propertyModel->setValueMap(properties);
}

QString PropertiesDialog::getPolicy() {
    properties = propertyModel->valueMap();
    return properties[itemPropertyNames.at(0)].toString();
}

int PropertiesDialog::getFixedCount() {
    properties = propertyModel->valueMap();
    return properties[itemPropertyNames.at(1)].toInt();
}
