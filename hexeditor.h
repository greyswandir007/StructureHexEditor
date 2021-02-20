#ifndef HEXEDITOR_H
#define HEXEDITOR_H

#include <QFrame>
#include <QByteArray>
#include <QAbstractTableModel>
#include <QScrollBar>
#include <QItemSelectionModel>

#include <components/structurebytearray.h>

enum ResizeModes {
    HEX_RESIZE_MODE_DYNAMIC, HEX_RESIZE_MODE_POW_BY_TWO, HEX_RESIZE_MODE_FIXED
};

class HexModel;
class HexItemDelegate;

namespace Ui {
class HexEditor;
}

class HexEditor : public QFrame {
    Q_OBJECT

public:
    explicit HexEditor(QWidget *parent = nullptr);
    ~HexEditor();
    void loadFile(QString filename);
    int row();
    int column();
    void resizeTable();
    bool isImported();
    bool isExported();

    StructureByteArray selectedData();

    QScrollBar *getScrollbar();
    void saveFileAs(QString filename);
    void saveFile();
    QItemSelectionModel *selectionModel();

    void copy(int copy_addr, StructureByteArray *copy_from, int start_idx, int length);
    void resizeEvent(QResizeEvent* event);
    void appendBinaryFile(QString filename, int offset);

    unsigned int getAddress (int row, int col);
    unsigned int getCurrentAddress ();

    void setBinaryData(StructureByteArray binaryData);
    StructureByteArray *getBinaryData();
    void setCompareData(StructureByteArray *compareData);
    StructureByteArray *getCompareData();

    unsigned int getColumnCount() const;
    void setColumnCount(unsigned int value);
    int getResizeMode() const;
    void setResizeMode(int value);

    StructureByteArray getBinaryDataByIndex(const QModelIndex &index, int length);
    StructureByteArray getBinaryDataByIndex(int offset, int length);

    void setBinaryData(QByteArray data);

signals:

public slots:
    void currentCellChanged(const QModelIndex &current, const QModelIndex &prev);

private:
    void updateColumnCount();

    Ui::HexEditor *ui;
    HexModel *model;
    HexItemDelegate *delegate;
    bool imported;
    bool exported;
    StructureByteArray binaryData;
    StructureByteArray *compareData;
    unsigned int columnCount;
    int resizeMode;
    QString filename;
    int charWidth;
};

#endif // HEXEDITOR_H
