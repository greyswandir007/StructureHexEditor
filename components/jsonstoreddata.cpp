#include "jsonstoreddata.h"
#include "jsonstoreddatahelper.h"
#include <QDebug>

JsonStoredData::JsonStoredData(JsonStoredData *parent) {
    this->parent = parent;
    if (parent != nullptr) {
        resolved = parent->resolved;
    }
}

JsonStoredData::JsonStoredData(JsonStoredData *parent, int type, QString name) {
    this->parent = parent;
    this->type = type;
    this->name = name;
    if (parent != nullptr) {
        resolved = parent->resolved;
    }
    if (type >= CHAR_TYPE && type <= LDOUBLE_TYPE) {
        size = fixedTypesSize[type];
    }
}

JsonStoredData::JsonStoredData(JsonStoredData *parent, unsigned int offset, int type, QString name, int arrayIndex, unsigned int size,
                               unsigned int count) {
    this->size = size;
    this->offset = offset;
    this->type = type;
    this->name = name;
    this->count = count;
    this->arrayIndex = arrayIndex;
    if (type >= CHAR_TYPE && type <= LDOUBLE_TYPE) {
        this->size = fixedTypesSize[type];
    }
    if (parent != nullptr) {
        if (arrayIndex >= 0) {
            if (parent->itemSize != 0) {
                this->offset = parent->itemSize * static_cast<unsigned int>(arrayIndex);
            } else {
                this->offset = 0;
                for (int i = 0; i < arrayIndex && i < parent->fields.size(); i++) {
                    this->offset += fields.at(i)->getSize();
                }
            }
        }
        this->parent = parent;
        globalOffset = parent->globalOffset + this->offset;
        fullName = parent->fullName.isEmpty() ? name : parent->fullName + "." + name;
        resolved = parent->resolved;
    }
    after = globalOffset + size;
}

unsigned int JsonStoredData::getSize() const {
    return size;
}

void JsonStoredData::setSize(unsigned int value) {
    size = value;
}

unsigned int JsonStoredData::getOffset() const {
    return offset;
}

void JsonStoredData::setOffset(unsigned int value) {
    offset = value;
}

unsigned int JsonStoredData::getGlobalOffset() const {
    return globalOffset;
}

void JsonStoredData::setGlobalOffset(unsigned int value) {
    globalOffset = value;
}

unsigned int JsonStoredData::getAfter() const {
    return after;
}

void JsonStoredData::setAfter(unsigned int value) {
    after = value;
}

int JsonStoredData::getType() const {
    return type;
}

void JsonStoredData::setType(int value) {
    type = value;
}

QString JsonStoredData::getName() const {
    return name;
}

void JsonStoredData::setName(const QString &value) {
    name = value;
}

QString JsonStoredData::getFullName() const {
    return fullName;
}

void JsonStoredData::setFullName(const QString &value) {
    fullName = value;
}

QVariant JsonStoredData::getValue() const {
    return value;
}

void JsonStoredData::setValue(const QVariant &value) {
    this->value = value;
}

unsigned int JsonStoredData::getCount() const {
    return count;
}

void JsonStoredData::setCount(unsigned int value) {
    count = value;
}

unsigned int JsonStoredData::getItemSize() const {
    return itemSize;
}

void JsonStoredData::setItemSize(unsigned int value) {
    itemSize = value;
}

int JsonStoredData::getArrayIndex() const {
    return arrayIndex;
}

void JsonStoredData::setArrayIndex(int value) {
    arrayIndex = value;
}

bool JsonStoredData::getIsValid() const {
    return isValid;
}

void JsonStoredData::setIsValid(bool value) {
    isValid = value;
}

QString JsonStoredData::getCheckValue() const {
    return checkValue;
}

void JsonStoredData::setCheckValue(const QString &value)
{
    checkValue = value;
}

QList<JsonStoredData *> JsonStoredData::getFields() const {
    return fields;
}

void JsonStoredData::appendField(JsonStoredData *field) {
    fields.append(field);
}

void JsonStoredData::freeData() {
    for (JsonStoredData *field : fields) {
        field->freeData();
        delete field;
    }
    fields.clear();
}

void JsonStoredData::updateOffset() {
    if (!resolved) {
        return;
    }
    if ((type == OBJECT_TYPE || (type == ARRAY_TYPE && itemSize == 0)) && !fields.isEmpty() && sizeReference.isEmpty()) {
        auto last = fields.at(0);
        for(auto data : fields) {
            if (data->getOffset() > last->getOffset()) {
                last = data;
            }
        }
        size = last->getOffset() + last->getSize();
    }
    if (parent != nullptr) {
        if (arrayIndex >= 0 && offsetReference.isEmpty()) {
            if (parent->itemSize == 0) {
                offset = 0;
                for (int i = 0; i < arrayIndex && i < parent->getFields().size(); i++) {
                    offset += parent->getFields().at(i)->getSize();
                }
            } else {
                offset = parent->itemSize * static_cast<unsigned int>(arrayIndex);
            }
        }
        globalOffset = parent->globalOffset + offset;
    }
    after = globalOffset + size;
    for(JsonStoredData *data : fields) {
        data->updateOffset();
    }
}

void JsonStoredData::reOrderFields() {
    std::sort(fields.begin(), fields.end(), offsetCompare);
}

JsonStoredData *JsonStoredData::getParent() const {
    return parent;
}

JsonStoredData *JsonStoredData::getRoot() {
    return parent == nullptr ? this : parent->getRoot();
}

void JsonStoredData::readDataValues(StructureByteArray *binary) {
    JsonStoredDataHelper::readDataValue(this, binary);
    for(JsonStoredData *field : fields) {
        field->readDataValues(binary);
    }
}

QString JsonStoredData::getSizeReference() const {
    return sizeReference;
}

void JsonStoredData::setSizeReference(const QString &value) {
    sizeReference = value;
    if (!value.isEmpty()) {
        resolved = false;
    }
}

QString JsonStoredData::getOffsetReference() const {
    return offsetReference;
}

void JsonStoredData::setOffsetReference(const QString &value) {
    offsetReference = value;
    if (!value.isEmpty()) {
        resolved = false;
    }
}

QString JsonStoredData::getCountReference() const {
    return countReference;
}

void JsonStoredData::setCountReference(const QString &value) {
    countReference = value;
    if (!value.isEmpty()) {
        resolved = false;
    }
}

QString JsonStoredData::getItemSizeReference() const {
    return itemSizeReference;
}

void JsonStoredData::setItemSizeReference(const QString &value) {
    itemSizeReference = value;
    if (!value.isEmpty()) {
        resolved = false;
    }
}

bool JsonStoredData::getResolved() const {
    return resolved;
}

void JsonStoredData::setResolved(bool value) {
    resolved = value;
}

bool JsonStoredData::resolveReferences(StructureByteArray *binary, bool *update) {
    bool fail = false;
    bool upd = false;
    updateReference(&size, &fail, &upd, sizeReference);
    updateReference(&offset, &fail, &upd, offsetReference);
    updateReference(&count, &fail, &upd, countReference);
    updateReference(&itemSize, &fail, &upd, itemSizeReference);
    if (!displayNameReference.isEmpty()) {
        JsonStoredData *data = JsonStoredDataHelper::findDataByName(this, displayNameReference);
        if (data != nullptr) {
            QString value = data->getValue().toString();
            if (!value.isEmpty() && value.compare(displayName) != 0) {
                displayName = value;
                *update = true;
            }
        }
    }
    if (update != nullptr) {
        *update = upd;
    }
    if (fail) {
        return false;
    }
    resolved = true;
    if (upd) {
        updateOffset();
        if (type == ARRAY_TYPE) {
            if (fields.size() == 1 && count != 1) {
                fields[0]->updateOffset();
                fields[0]->resolveReferences(binary);
                for (unsigned int i = 1; i < count; i++) {
                    auto field = JsonStoredDataHelper::createCopy(fields[0]);
                    field->setArrayIndex(static_cast<int>(i));
                    field->updateOffset();
                    field->setName(QString("item[%1]").arg(i));
                    field->updateFullNames();
                    field->resolveReferences(binary);
                    fields.append(field);
                }
            }
        }
    }
    if (binary != nullptr) {
        JsonStoredDataHelper::readDataValue(this, binary);
    }
    return true;
}

int JsonStoredData::resolveAllReferences(StructureByteArray *binary) {
    int count = 0;
    bool update = false;
    if (resolveReferences(binary, &update)) {
        if (update) {
            count++;
        }
        for (auto field : fields) {
            count += field->resolveAllReferences(binary);
        }
    }
    return count;
}

void JsonStoredData::updateReference(unsigned int *value, bool *fail, bool *update, QString reference) {
    if (!reference.isEmpty()) {
        bool ok = false;
        unsigned int v = JsonStoredDataHelper::findValue(this, reference, &ok);
        if (!ok) {
            *fail = true;
        } else if (*value != v) {
            *value = v;
            *update = true;
        }
    }
}

QString JsonStoredData::getDisplayNameReference() const {
    return displayNameReference;
}

void JsonStoredData::setDisplayNameReference(const QString &value) {
    displayNameReference = value;
}

QString JsonStoredData::getDisplayName() const {
    return displayName;
}

void JsonStoredData::setDisplayName(const QString &value) {
    displayName = value;
}

void JsonStoredData::setParent(JsonStoredData *value) {
    parent = value;
}

void JsonStoredData::updateFullNames() {
    if (parent != nullptr) {
        fullName = parent->fullName.isEmpty() ? name : parent->fullName + "." + name;
        for (auto field : fields) {
            field->updateFullNames();
        }
    }
}
