#include "hexeditor.h"
#include "ui_hexeditor.h"
#include "hexmodel.h"
#include "hexmanager.h"

#include <QFont>
#include <QFontMetrics>
#include <QFontDatabase>
#include <QFile>
#include <QDebug>
#include <QHeaderView>
#include <QResizeEvent>

#include <QtMath>

#include <components/bbytearray.h>

HexEditor::HexEditor(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::HexEditor) {
    ui->setupUi(this);
    compareData = nullptr;
    columnCount = 8;
    resizeMode = HEX_RESIZE_MODE_POW_BY_TWO;
    imported = false;

    ui->hexEditor->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->hexEditor->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    QFontMetrics metrics(ui->hexEditor->font());
    charWidth = metrics.averageCharWidth();

    model = new HexModel(this);
    ui->hexEditor->setModel(model);
    delegate = new HexItemDelegate(this);
    ui->hexEditor->setItemDelegate(delegate);

    //Изначально виджету не с чем сравнивать свои данные
    resizeTable();
    connect(ui->hexEditor->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(currentCellChanged(const QModelIndex&, const QModelIndex&)));
    ui->splitter->setStretchFactor(0, 8);
    ui->splitter->setStretchFactor(1, 1);
    ui->horizontalLayout->setStretchFactor(ui->formLayout, 1);
    ui->horizontalLayout->setStretchFactor(ui->formLayout_2, 3);
    ui->horizontalLayout->setStretchFactor(ui->formLayout_3, 4);
}

HexEditor::~HexEditor() {
    if (binaryData.size() > 0) {
        binaryData.clear();
    }
    delete ui;
}

// Сохранить файл в зависимости от флага imported
// для "импортированного" файла совершить экспорт, иначе - сохранить бинарно
void HexEditor::saveFile() {
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QString comment;

        // Импорт из hex или бинарный?
        // Сохранять бинарно или текстово?
        if (imported) {
            QStringList save_hex = HexManager::bin2Hex(binaryData, 8);
            comment = tr("Exported to HEX");
            foreach(QString line, save_hex) {
                f.write(line.toLocal8Bit());
            }
            exported = true;
        } else {
            comment = tr("Binary");
            f.write(binaryData);
        }
        ui->fileName->setText(filename + " (" + comment + ")");
        f.close();
    }
}

// Сохранить файл в зависимости от расширения в имени файла
// для "hex" - совершить экспорт
void HexEditor::saveFileAs(QString filename) {
    this->filename = filename;
    int l = filename.length();
    QFile f(filename);
    QString ext = filename.mid(l - 4, 4);
    QString comment;

    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        //Сохранять бинарно или текстово?
        if (ext.compare(".hex", Qt::CaseInsensitive) == 0) {
            QStringList save_hex = HexManager::bin2Hex(binaryData, 8);
            comment = tr("Exported to HEX");
            foreach(QString line, save_hex) {
                f.write(line.toLocal8Bit());
            }
            exported = true;
        } else {
            comment = tr("Binary");
            f.write(binaryData);
        }
        ui->fileName->setText(filename + " (" + comment + ")");
        f.close();
    }
}

void HexEditor::loadFile(QString filename) {
    imported = false;
    exported = false;
    this->filename = filename;

    if (binaryData.size() > 0) {
        binaryData.clear();
    }

    // Расширение не HEX ли?
    if (filename.length() > 4) {
        int l = filename.length();
        QString ext = filename.mid(l - 4, 4);
        if (ext.compare(".hex", Qt::CaseInsensitive) == 0) {
            binaryData = BByteArray(HexManager::loadAsHex(filename, &imported));
            if (imported) {
                qDebug()  << "import of HEX succeeeful";
                ui->fileName->setText(filename + " (" + tr("HEX import") + ")");
                imported = true;
            }
        }
    }

    // Если HEX-импорт не случился
    if (!imported) {
        QFile f(filename);
        if (f.open(QIODevice::ReadOnly)) {
            binaryData = BByteArray(f.readAll());
            if (binaryData.size() > 0) {
                model->updateData();
                resizeTable();
            }
            f.close();
            ui->fileName->setText(filename + " (" + tr("Binary") + ")");
        }
    }
}

void HexEditor::resizeTable() {
    int w = ui->hexEditor->width() - 10 - charWidth * (2 * columnCount + 13);
    int cWidth = w / (2 * columnCount);

    ui->hexEditor->horizontalHeader()->resizeSection(0, charWidth * 10);
    ui->hexEditor->horizontalHeader()->resizeSection(columnCount + 1, charWidth * (columnCount + 1));

    for (int i = 0; i < columnCount; i ++) {
        ui->hexEditor->horizontalHeader()->resizeSection(i + 1, cWidth * 2 + charWidth);
    }

}

void HexEditor::resizeEvent(QResizeEvent* event) {
    event->isAccepted();
    updateColumnCount();
}

void HexEditor::appendBinaryFile(QString filename, int offset) {
    QFile f(filename);
    if (f.open(f.ReadOnly)) {
        qint64 filesize = f.size();
        if (filesize > 0) {
            QByteArray b_tmp = f.read(filesize);
            binaryData.replace(offset, b_tmp.length(), b_tmp);
            model->updateData();
            this->resizeTable();
        }
    }
}

QScrollBar* HexEditor::getScrollbar() {
    return ui->hexEditor->verticalScrollBar();
}

int HexEditor::row() {
    return ui->hexEditor->currentIndex().row();
}

int HexEditor::column() {
    return ui->hexEditor->currentIndex().column();
}

void HexEditor::currentCellChanged(const QModelIndex &current, const QModelIndex &prev) {
    int offset = (current.row() * columnCount) + current.column() - 1;
    ui->charValue->setText(QString().sprintf("%i", binaryData.at(offset)));
    ui->ucharValue->setText(QString().sprintf("%u", binaryData.ucharValueAt(offset)));
    ui->shortValue->setText(QString().sprintf("%i", binaryData.shortValueAt(offset)));
    ui->ushortValue->setText(QString().sprintf("%u", binaryData.ushortValueAt(offset)));
    ui->longValue->setText(QString().sprintf("%li", binaryData.longValueAt(offset)));
    ui->ulongValue->setText(QString().sprintf("%lu", binaryData.ulongValueAt(offset)));
    ui->longLongValue->setText(QString().sprintf("%lli", binaryData.longLongValueAt(offset)));
    ui->ulongLongValue->setText(QString().sprintf("%llu", binaryData.ulongLongValueAt(offset)));
    ui->floatValue->setText(QString().sprintf("%g", binaryData.floatValueAt(offset)));
    ui->doubleValue->setText(QString().sprintf("%g", binaryData.doubleValueAt(offset)));
    ui->ldoubleValue->setText(QString().sprintf("%lg", binaryData.ldoubleValueAt(offset)));
    ui->stringValue->setText(binaryData.stringAt(offset, 20));
    // Обновить последний столбец - тот что графически созданный ASCII
    model->updateCell(current.row(), columnCount + 1);
    // И если сменилась строка - то и убрать нарисованный "курсор" со старой строки
    if (prev.row() != current.row()) {
        model->updateCell(prev.row(), columnCount + 1);
    }
}

void HexEditor::updateColumnCount() {
    //Для фиксированного числа столбцов не пересчитываем
    if (resizeMode != HEX_RESIZE_MODE_FIXED) {
        int w_frame = ui->hexEditor->geometry().width();

        /* И так, чтобы дважды по двойному полю плюс "икс" столбцов помещалось без помех */
        int count = ((w_frame - 10) / charWidth - 11) / 4;

        //Уж меньше 4х столбцов совсем несерьёзно!
        if (count < 3) {
            count = 3;
        }

        // Для HEXWIDGET_RESIZE_DYNAMIC дальше не пересчитываем,
        // для HEXWIDGET_RESIZE_BY_TWO считаем ближайшую степень двойки
        if (resizeMode == HEX_RESIZE_MODE_POW_BY_TWO) {
            float baseFloat = static_cast<float>(qLn(static_cast<double>(count)) / qLn(2.0));
            int base2 = static_cast<int>(baseFloat);
            count = base2 < 2 ? 2 : (1 << base2);
        }
        columnCount = count;
    }
    model->updateData();
    resizeTable();
}

QItemSelectionModel*  HexEditor::selectionModel() {
    return ui->hexEditor->selectionModel();
}

void HexEditor::copy(int copy_addr, BByteArray *copy_from, int start_idx, int length) {
    QByteArray b_tmp = copy_from->mid(start_idx, length);
    binaryData.replace(copy_addr, length, b_tmp);
    model->updateData();
    this->resizeTable();
}

bool HexEditor::isImported() {
    return imported;
}

bool HexEditor::isExported() {
    return exported;
}

BByteArray HexEditor::selectedData() {
    BByteArray result;
    QModelIndexList l = ui->hexEditor->selectionModel()->selectedIndexes();
    for (QModelIndex idx : l) {
        result.append(static_cast<char>(this->model->getByte(idx)));
    }
    return result;
}

unsigned long HexEditor::getAddress (int row, int col) {
    return static_cast<unsigned long>(row * columnCount + col - 1);
}

unsigned long HexEditor::getCurrentAddress () {
    return getAddress (ui->hexEditor->currentIndex().row(), ui->hexEditor->currentIndex().column());
}

void HexEditor::setBinaryData(BByteArray binaryData) {
    this->binaryData = binaryData;
}

BByteArray *HexEditor::getBinaryData() {
    return &binaryData;
}

void HexEditor::setCompareData(BByteArray *compareData) {
    this->compareData = compareData;
}

BByteArray *HexEditor::getCompareData() {
    return compareData;
}

int HexEditor::getColumnCount() const {
    return columnCount;
}

void HexEditor::setColumnCount(int value) {
    columnCount = value;
}

int HexEditor::getResizeMode() const {
    return resizeMode;
}

void HexEditor::setResizeMode(int value) {
    resizeMode = value;
    updateColumnCount();
}

BByteArray HexEditor::getBinaryDataByIndex(const QModelIndex &index, int length) {
    return getBinaryDataByIndex((index.row() * columnCount) + index.column() - 1, length);
}

BByteArray HexEditor::getBinaryDataByIndex(int offset, int length) {
    return binaryData.mid(offset, length);
}
