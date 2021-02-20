#include "finddialog.h"
#include "ui_finddialog.h"

#include <QRegularExpression>
#include <QTextCodec>

#include <components/structurebytearray.h>

FindDialog::FindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindDialog)
{
    ui->setupUi(this);
}

FindDialog::~FindDialog() {
    delete ui;
}

StructureByteArray FindDialog::getSearchValue() {
    StructureByteArray array;
    array.setHexAt(0, ui->findHexEdit->text());
    return array;
}

bool FindDialog::continueSearch() {
    return ui->continueSearchButton->isChecked();
}

void FindDialog::on_findTextEdit_textChanged(const QString &arg1) {
    if (!textChanged) {
        textChanged = true;
        QTextCodec *codec = QTextCodec::codecForName("CP1251");
        StructureByteArray array = StructureByteArray(codec->fromUnicode(arg1));
        ui->findHexEdit->setText(array.hexAt(0, static_cast<unsigned int>(array.size())));
    } else {
        textChanged = false;
    }
}

void FindDialog::on_findHexEdit_textChanged(const QString &arg1) {
    QRegularExpression regexp = QRegularExpression(QString("[0-9A-Fa-f]*"));
    if (regexp.match(arg1).hasMatch()) {
        QString result;
        QRegularExpressionMatchIterator matchIterator = regexp.globalMatch(arg1);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            result += match.captured();
        }
        if (result.compare(arg1) != 0) {
            ui->findHexEdit->setText(result);
            return;
        }
    }
    if (!textChanged) {
        textChanged = true;
        StructureByteArray array;
        array.setHexAt(0, arg1);
        QTextCodec *codec = QTextCodec::codecForName("CP1251");
        ui->findTextEdit->setText(codec->toUnicode(array));
    } else {
        textChanged = false;
    }

}
