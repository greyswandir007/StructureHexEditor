#ifndef HEXMODEL_H
#define HEXMODEL_H

#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QFontMetrics>
#include <QFont>
#include <QPainter>
#include <QPixmap>

#include "hexeditor.h"

class HexModel : public QAbstractTableModel {
public:
    HexModel(HexEditor *p);

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const ;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;    
    quint8 getByte(QModelIndex &index);
    int getAddr(QModelIndex &index);

    void updateCell(int row, int column);
    void updateData();

    QFontMetrics    *fm;
private:
    HexEditor *hexEditor;
    QPixmap *pix;
    QPainter *painter;
    QFont fixed_font;
};

class HexItemDelegate : public QItemDelegate {
public:
    HexItemDelegate(HexEditor* parent = nullptr);

    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
private:
    HexEditor *hexEditor;
    QFontMetrics *fm;
    QFont fixed_font;
};

#endif // HEXMODEL_H
