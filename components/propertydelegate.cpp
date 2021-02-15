#include "propertydelegate.h"
#include <QDebug>
#include <QComboBox>

PropertyDelegate::PropertyDelegate() {
}

PropertyDelegate::~PropertyDelegate() {
}

QWidget *PropertyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const {
    qDebug() << "create editor for index row:" << index.row() << ", col:" << index.column();
    if (index.row() == COMBO_BOX_COLUMN) {
        QComboBox *comboBox = new QComboBox(parent);
        QStringList values;
        values << "Dynamic" << "Pow by 2" << "Fixed";
        comboBox->addItems(values);
        return comboBox;
    }
    return QItemDelegate::createEditor(parent, option, index);
}

void PropertyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    if (index.row() == COMBO_BOX_COLUMN) {
        QComboBox *edit = static_cast<QComboBox*>(editor);
        int i = edit->findText(index.model()->data(index, Qt::EditRole).toString());
        edit->setCurrentIndex(i);
    } else {
        QItemDelegate::setEditorData(editor, index);
    }
}

void PropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    if (index.row() == COMBO_BOX_COLUMN) {
        model->setData(index, static_cast<QComboBox*>(editor)->currentText());
    } else {
        QItemDelegate::setModelData(editor, model, index);
    }
}

void PropertyDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const {
    editor->setGeometry(option.rect);
}
