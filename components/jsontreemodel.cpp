#include "jsonstoreddatahelper.h"
#include "jsontreemodel.h"

JsonTreeModel::JsonTreeModel(QObject *parent) :
    QAbstractTableModel(parent) {
    headers << tr("Name") << tr("Full Name") << tr("Offset") << tr("Global Offset") << tr("Size") << tr("Value")
            << tr("Type") << tr("Count") << tr("Item Size") << tr("References");
}

void JsonTreeModel::setRoot(JsonStoredData *value) {
    root = value;
    updateData();
}

QVariant JsonTreeModel::data(const QModelIndex &index, int role) const {
    if (root == nullptr) {
        return QVariant();
    }
    if (index.isValid() && index.internalPointer() != nullptr && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        JsonStoredData *data = static_cast<JsonStoredData *>(index.internalPointer());
        switch (index.column()) {
        case 0: return data->getName();
        case 1: return data->getFullName();
        case 2: return JsonStoredDataHelper::ulongToHex(data->getOffset());
        case 3: return JsonStoredDataHelper::ulongToHex(data->getGlobalOffset());
        case 4: return QString().sprintf("%u", data->getSize());
        case 5:
            if (data->getValue().toByteArray().size() > 100) {
                return data->getValue().toByteArray().mid(0, 20);
            } else {
                return data->getValue();
            }
        case 6: return jsonTypes.at(data->getType());
        case 7: return QString().sprintf("%u", data->getCount());
        case 8: return QString().sprintf("%u", data->getItemSize());
        case 9:
            QString references;
            if (!data->getSizeReference().isEmpty()) {
                references = "size: " + data->getSizeReference();
            }
            if (!data->getOffsetReference().isEmpty()) {
                if (!references.isEmpty()) {
                    references += ", ";
                }
                references += "offset: " + data->getOffsetReference();
            }
            if (!data->getCountReference().isEmpty()) {
                if (!references.isEmpty()) {
                    references += ", ";
                }
                references += "count: " + data->getCountReference();
            }
            if (!data->getItemSizeReference().isEmpty()) {
                if (!references.isEmpty()) {
                    references += ", ";
                }
                references += "itemSize: " + data->getItemSizeReference();
            }
            return references;
        }
    }
    return QVariant();
}

bool JsonTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    return false;
}

int JsonTreeModel::rowCount(const QModelIndex &parent) const {
    if (root == nullptr) {
        return 0;
    }
    JsonStoredData *parentItem = root;
    if (parent.isValid()) {
        parentItem = static_cast<JsonStoredData*>(parent.internalPointer());
    }
    return parentItem->getFields().size();
}

int JsonTreeModel::columnCount(const QModelIndex &parent) const {
    return headers.size();
}

QVariant JsonTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        return headers.at(section);
    } else {
        return QString("%1").arg(section + 1);
    }
}

Qt::ItemFlags JsonTreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QModelIndex JsonTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (root == nullptr) {
        return QModelIndex();
    }
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    JsonStoredData *parentItem = root;
    if (parent.isValid()) {
        parentItem = static_cast<JsonStoredData *>(parent.internalPointer());
    }
    if (row < parentItem->getFields().size()) {
        return createIndex(row, column, parentItem->getFields().at(row));
    } else {
        return QModelIndex();
    }
}

QModelIndex JsonTreeModel::parent(const QModelIndex &index) const {
    if (root == nullptr) {
        return QModelIndex();
    }
    if (!index.isValid()) {
        return QModelIndex();
    }
    JsonStoredData *parentItem = static_cast<JsonStoredData *>(index.internalPointer())->getParent();
    if (parentItem == root) {
        return QModelIndex();
    }
    int row = 0;
    if (parentItem->getParent() != nullptr) {
        row = parentItem->getParent()->getFields().indexOf(parentItem);
    }
    return createIndex(row, 0, parentItem);
}

void JsonTreeModel::updateData() {
    QModelIndex zero_index = createIndex(0, 0);
    QModelIndex end_index = createIndex(this->rowCount(), headers.size());

    emit layoutAboutToBeChanged();
    emit layoutChanged();
    emit dataChanged(zero_index, end_index);
}
