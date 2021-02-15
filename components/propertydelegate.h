#ifndef PROPERTYDELEGATE_H
#define PROPERTYDELEGATE_H

#include <QItemDelegate>

class PropertyDelegate : public QItemDelegate {
    Q_OBJECT
public:
    PropertyDelegate();
    virtual ~PropertyDelegate();

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;
private:
    static const int COMBO_BOX_COLUMN = 0;
};

#endif // PROPERTYDELEGATE_H
