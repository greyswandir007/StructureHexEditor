#include "gotodialog.h"
#include "ui_gotodialog.h"

GoToDialog::GoToDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoToDialog) {
    ui->setupUi(this);
}

GoToDialog::~GoToDialog() {
    delete ui;
}

unsigned int GoToDialog::getValue() {
    return static_cast<unsigned int>(ui->addressValueSpin->value());
}
