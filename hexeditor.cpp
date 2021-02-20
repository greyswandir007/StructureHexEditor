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

#include <components/structurebytearray.h>

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
            binaryData = StructureByteArray(HexManager::loadAsHex(filename, &imported));
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
            binaryData = StructureByteArray(f.readAll());
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
        qint64 fileSize = f.size();
        if (fileSize > 0) {
            QByteArray b_tmp = f.read(fileSize);
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
    if (!binaryData.isEmpty()) {
        unsigned int row = static_cast<unsigned int>(current.row());
        unsigned int offset = row * static_cast<unsigned int>(columnCount) + static_cast<unsigned int>(current.column()) - 1;
        ui->charValue->setText(QString().sprintf("%i", binaryData.at(static_cast<int>(offset))));
        ui->ucharValue->setText(QString().sprintf("%u", binaryData.ucharAt(offset)));
        ui->shortValue->setText(QString().sprintf("%i", binaryData.shortAt(offset)));
        ui->ushortValue->setText(QString().sprintf("%u", binaryData.ushortAt(offset)));
        ui->longValue->setText(QString().sprintf("%i", binaryData.longAt(offset)));
        ui->ulongValue->setText(QString().sprintf("%u", binaryData.ulongAt(offset)));
        ui->longLongValue->setText(QString().sprintf("%lli", binaryData.longLongAt(offset)));
        ui->ulongLongValue->setText(QString().sprintf("%llu", binaryData.ulongLongAt(offset)));
        ui->floatValue->setText(QString().sprintf("%g", binaryData.floatAt(offset)));
        ui->doubleValue->setText(QString().sprintf("%g", binaryData.doubleAt(offset)));
        ui->ldoubleValue->setText(QString().sprintf("%lg", binaryData.ldoubleAt(offset)));
        ui->stringValue->setText(binaryData.stringAt(offset, 20));
    }
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

void HexEditor::copy(int copy_addr, StructureByteArray *copy_from, int start_idx, int length) {
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

StructureByteArray HexEditor::selectedData() {
    StructureByteArray result;
    QModelIndexList l = ui->hexEditor->selectionModel()->selectedIndexes();
    for (QModelIndex idx : l) {
        result.append(static_cast<char>(this->model->getByte(idx)));
    }
    return result;
}

unsigned int HexEditor::getAddress(int row, int col) {
    return static_cast<unsigned int>(row * columnCount) + static_cast<unsigned int>(col) - 1;
}

unsigned int HexEditor::getCurrentAddress() {
    return getAddress (ui->hexEditor->currentIndex().row(), ui->hexEditor->currentIndex().column());
}

void HexEditor::setBinaryData(StructureByteArray binaryData) {
    this->binaryData = binaryData;
}

StructureByteArray *HexEditor::getBinaryData() {
    return &binaryData;
}

void HexEditor::setCompareData(StructureByteArray *compareData) {
    this->compareData = compareData;
}

StructureByteArray *HexEditor::getCompareData() {
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

StructureByteArray HexEditor::getBinaryDataByIndex(const QModelIndex &index, int length) {
    return getBinaryDataByIndex((index.row() * static_cast<int>(columnCount)) + index.column() - 1, length);
}

StructureByteArray HexEditor::getBinaryDataByIndex(int offset, int length) {
    return binaryData.mid(offset, length);
}

void HexEditor::setBinaryData(QByteArray data) {
    binaryData = StructureByteArray(data);
    model->updateData();
    resizeTable();
}

void HexEditor::goToAddress(unsigned int address) {
    if (binaryData.isEmpty()) {
        return;
    }
    if (address >= static_cast<unsigned int>(binaryData.size())) {
        address = static_cast<unsigned int>(binaryData.size()) - 1;
    }
    int row = static_cast<int>(address / static_cast<unsigned int>(columnCount));
    int col = static_cast<int>(address % static_cast<unsigned int>(columnCount)) + 1;
    ui->hexEditor->selectionModel()->setCurrentIndex(model->index(row, col),
                                                     QItemSelectionModel::Current | QItemSelectionModel::Select);

}
