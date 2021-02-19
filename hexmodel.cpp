
#include <QDebug>
#include <QFont>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QPainter>
#include <QRect>
#include <QLabel>

#include "hexmodel.h"


HexModel::HexModel(HexEditor *p) {
    hexEditor = p;
    pix = nullptr;
    painter = nullptr;
    fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    fm = new QFontMetrics(fixed_font);
}

int HexModel::rowCount(const QModelIndex &parent) const {
    if (parent == QModelIndex()) {
        if (hexEditor->getBinaryData()->size() == 0) {
            return 1;
        }
        int bytes = hexEditor->getBinaryData()->size();
        int columnCount = hexEditor->getColumnCount();
        return bytes / columnCount + (bytes % columnCount != 0);
    }
    return 0;
}


int HexModel::columnCount(const QModelIndex &parent) const {
    if (parent == QModelIndex()) {
        return hexEditor->getBinaryData()->size() == 0 ? 2 : 2 + hexEditor->getColumnCount();
    }
    return 0;
}

quint8 HexModel::getByte(QModelIndex &index) {
    int row = index.row();
    int col = index.column();
    int position = row * hexEditor->getColumnCount() + col - 1;
    if (position < hexEditor->getBinaryData()->size()) {
        return static_cast<quint8>(hexEditor->getBinaryData()->at(position));
    }
    return 0xFF;
}

int HexModel::getAddr(QModelIndex &index) {
    int row = index.row();
    int col = index.column();
    return row * hexEditor->getColumnCount() + col - 1;
}

QVariant HexModel::data(const QModelIndex &index, int role) const {
    if (hexEditor->getBinaryData()->size() > 0) {
        unsigned int row = static_cast<unsigned int>(index.row());
        unsigned int col = static_cast<unsigned int>(index.column());

        // Вот здесь отрисовывать ASCII составляющую
        if (role == Qt::DecorationRole) {
        }

        unsigned int columnCount = hexEditor->getColumnCount();
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            if (col > 0) {
                // Последний столбец - это уже ASCII, тут пустышка
                if (col == 1 + columnCount) {
                    return hexEditor->getBinaryData()->mid(row * columnCount, columnCount);
                }
                unsigned long position = (row * columnCount) + col - 1;
                if (position < (unsigned long) hexEditor->getBinaryData()->size()) {
                    quint8 byte = hexEditor->getBinaryData()->at(position);
                    QString result;
                    return result.sprintf("%02X", byte);
                }
                return QString("xx");
            } else {
                unsigned int addr_high = ((row * columnCount) & 0xFFFF0000) >> 16;
                unsigned int addr_low = (row * columnCount) & 0xFFFF;

                return QString().sprintf("%04X:%04X", addr_high, addr_low);
            }
        }
    }
    return QVariant();
}


void HexModel::updateData() {
    QModelIndex zero_index = createIndex(0, 0);
    QModelIndex end_index = createIndex(this->rowCount(), columnCount());

    emit layoutAboutToBeChanged();
    emit layoutChanged();
    emit dataChanged(zero_index, end_index);
}

void HexModel::updateCell(int row, int column) {
    QModelIndex idx = this->createIndex(row, column);
    emit dataChanged(idx, idx);
}

bool HexModel::setData(const QModelIndex & index, const QVariant & value, int role) {
    if (role == Qt::EditRole) {
        int row = index.row();
        int col = index.column();

        unsigned int position = (row * hexEditor->getColumnCount()) + col - 1;

        QString new_value = value.toString();
        if (new_value.length() != 2)
            return false;
        else {
            bool okay;
            quint8 byte = new_value.toInt(&okay, 16);
            if (okay) {
                hexEditor->getBinaryData()->data()[position] = byte;
            }
            return okay;
        }
    }
    return true;
}

Qt::ItemFlags HexModel::flags(const QModelIndex &index) const {
    int row = index.row();
    int col = index.column();

    //Столбец адреса и столбец ASCII - не редактируются
    if ((col == 0) || col == (hexEditor->getColumnCount() + 1)) {
        return Qt::NoItemFlags;
    }

    long position = (row * hexEditor->getColumnCount()) + col - 1;
    if (position < hexEditor->getBinaryData()->size()) {
            return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
    } else {
        //Некратные байты за пределом загруженного файла - тоже не редактируются
        return Qt::NoItemFlags;
    }
}


QVariant HexModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return tr("Address");
        }
        if (section == hexEditor->getColumnCount() + 1) {
            return tr("ASCII view");
        }
        return QString().sprintf("%02X", section - 1);
    }
    return QVariant();
}


//И класс делегата ячейки
HexItemDelegate::HexItemDelegate(HexEditor *parent) {
    hexEditor = parent;
    fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    fm = new QFontMetrics(fixed_font);
}

void HexItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
    int     row, col, p_cols, p_row, p_col;
    row = index.row();
    col = index.column();
    p_cols = hexEditor->getColumnCount();
    p_row = hexEditor->row();
    p_col = hexEditor->column();

    painter->setBrush(QColor(200, 220, 200));
    painter->setPen(QColor(0, 100, 0));

    //Отрисовка "основной" части - то есть HEX
    if (col < (p_cols +1)) {        
        QString data =  index.data(Qt::DisplayRole).toString();
        QRect       r_field = fm->boundingRect(data);

        int W = option.rect.width();
        int w = r_field.width();
        int H = option.rect.height();
        int h = r_field.height();
        QPen    pen;
        QBrush  brush;

        //Если нужно сравнивать с другой панелью, тo, пропустив нулевой столбец...
        if (col > 0 && hexEditor->getCompareData() != nullptr) {
            int position = index.row() * hexEditor->getColumnCount() + index.column() - 1;
            int dataSize = std::min(hexEditor->getBinaryData()->size(), hexEditor->getCompareData()->size());
            pen.setColor(QColor(0, 150, 0));
            if (position < dataSize) {
                    if ((hexEditor->getBinaryData()->at(position) != hexEditor->getCompareData()->at(position))) {
                        pen.setColor(QColor(150, 0, 0));
                    }

            } else {
                pen.setColor(QColor(0, 0, 150));
            }
            painter->setPen(pen);
        }

        //И если это не нулевой столбец, и если нужно отобразить выделение
        if (col > 0) {
            if (option.state & QStyle::State_Selected) {
                brush.setColor(QColor(200, 220, 200));
                pen.setColor(QColor(0, 150, 0));
                brush.setStyle(Qt::BrushStyle::SolidPattern);
                painter->setBrush(brush);
                painter->setPen(pen);
                painter->drawRect(option.rect.x(), option.rect.y(), W-1, H-1);
                pen.setColor(QColor(0, 150, 0));
                painter->setPen(pen);
            }
        }
        painter->drawText(option.rect.x() + (W - w) / 2, option.rect.y() + H - h / 2,  data);
    } else {
        float h = fm->height();
        float H = option.rect.height();

        if (p_row == row) {
            float x = fm->averageCharWidth() * (p_col - 1);
            float w = fm->averageCharWidth();
            painter->drawRect(option.rect.x() + x, option.rect.y() + (H - h) / 2, w, fm->height() );
        }

        QByteArray  chunk = index.data(Qt::DisplayRole).toByteArray();        
        int bytes = chunk.size();
        for (int i = 0; i < bytes; i ++) {
            float x = fm->averageCharWidth() * i;
            char c = chunk.at(i);
            if (c < ' ') {
                c = '.';
            }
            painter->drawText(option.rect.x() + x, option.rect.y() + H - h / 2, QString().append(c));
        }
    }
}

QSize HexItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const {
    int col;
    if (option.direction) {}
    col = index.column();
    //Адрес
    if (col == 0) {
        QRect   r = fm->boundingRect(" FFFF:FFFF ");
        return QSize(r.width(), r.height());
    }

    //Сами HEX-значения
    if (col < hexEditor->getColumnCount() + 1){
        QRect r = fm->boundingRect(" FF ");
        return QSize(r.width(), r.height());
    }

    //И последняя колонка - это ASCII
    int chunk_size = index.data(Qt::DisplayRole).toByteArray().length();
    QString     s_chunk;
    s_chunk.fill('.', chunk_size + 2);
    QRect r = fm->boundingRect(s_chunk);
    return QSize(r.width(), r.height());
}

