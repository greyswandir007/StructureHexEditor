#include "jsonstoreddata.h"
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
}

JsonStoredData::JsonStoredData(JsonStoredData *parent, unsigned long offset, int type, QString name, int arrayIndex, unsigned long size,
                               unsigned long count) {
    this->size = size;
    this->offset = offset;
    this->type = type;
    this->name = name;
    this->count = count;
    this->arrayIndex = arrayIndex;
    if (parent != nullptr) {
        if (arrayIndex >= 0) {
            this->offset = parent->itemSize * static_cast<unsigned long>(arrayIndex);
        }
        this->parent = parent;
        globalOffset = parent->globalOffset + this->offset;
        fullName = parent->fullName.isEmpty() ? name : parent->fullName + "." + name;
        resolved = parent->resolved;
    }
    after = globalOffset + size;
}

unsigned long JsonStoredData::getSize() const {
    return size;
}

void JsonStoredData::setSize(unsigned long value) {
    size = value;
}

unsigned long JsonStoredData::getOffset() const {
    return offset;
}

void JsonStoredData::setOffset(unsigned long value) {
    offset = value;
}

unsigned long JsonStoredData::getGlobalOffset() const {
    return globalOffset;
}

void JsonStoredData::setGlobalOffset(unsigned long value) {
    globalOffset = value;
}

unsigned long JsonStoredData::getAfter() const {
    return after;
}

void JsonStoredData::setAfter(unsigned long value) {
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

unsigned long JsonStoredData::getCount() const {
    return count;
}

void JsonStoredData::setCount(unsigned long value) {
    count = value;
}

unsigned long JsonStoredData::getItemSize() const {
    return itemSize;
}

void JsonStoredData::setItemSize(unsigned long value) {
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

QString JsonStoredData::toString(int identCount) {
    QString result;
    QString smallIdent = QString().fill(' ', identCount);
    result = smallIdent + "{";
    QString ident = "\n" + smallIdent + "    ";
    result += ident + QString("\"size\":%1").arg(size) + ",";
    result += ident + "\"offset\":\"" + ulongToHex(offset) + "\",";
    result += ident + "\"globalOffset\":\"" + ulongToHex(globalOffset) + "\",";
    result += ident + "\"after\":\"" + ulongToHex(after) + "\",";
    result += ident + "\"type\":\"" + jsonTypes.at(type) + "\",";
    result += ident + "\"name\":\"" + name + "\",";
    result += ident + "\"fullName\":\"" + fullName + "\",";
    result += ident + "\"value\":\"" + value.toString() + "\",";

    result += ident + QString("\"count\":%1").arg(count) + ",";
    result += ident + "\"itemSize\":\"" + ulongToHex(itemSize) + "\",";
    result += ident + QString("\"arrayIndex\":%1").arg(arrayIndex) + ",";
    result += ident + "\"isValid\":\"" + (isValid ? "true" : "false") + "\",";
    result += ident + "\"checkValue\":\"" + checkValue + "\",";
    if (fields.isEmpty()) {
        result += ident + "fields:[]";
    } else {
        result += ident + "fields:[\n";
        for (int i = 0; i < fields.size(); i++) {
            result += fields[i]->toString(identCount + 8);
            if (i < fields.size() - 1) {
                result += ",\n";
            }
        }
        result += ident + "]";
    }
    result += "\n" + smallIdent + "}";
    return result;
}

void JsonStoredData::readDataValue(StructureByteArray *binary) {
    if (!resolved) {
        return;
    }
    for (JsonStoredData *field : fields) {
        field->readDataValue(binary);
    }
    switch(type) {
    case SIGNATURE_TYPE:
        if (checkValue.startsWith("0x")) {
            unsigned long sigSize = static_cast<unsigned long>((checkValue.size() - 2) / 2);
            QString hex = "0x" + binary->hexAt(globalOffset, sigSize);
            value = hex;
            isValid = hex.compare(checkValue, Qt::CaseInsensitive) == 0;
        } else {
            isValid = false;
        }
        break;
    case CHAR_TYPE:
        value = static_cast<int>(binary->at(static_cast<int>(globalOffset)));
        size = 1;
        break;
    case UCHAR_TYPE:
        value = binary->ucharAt(globalOffset);
        size = 1;
        break;
    case SHORT_TYPE:
        value = binary->shortAt(globalOffset);
        size = 2;
        break;
    case USHORT_TYPE:
        value = binary->ushortAt(globalOffset);
        size = 2;
        break;
    case LONG_TYPE:
        value = static_cast<int>(binary->longAt(globalOffset));
        size = 4;
        break;
    case ULONG_TYPE:
        value = static_cast<unsigned int>(binary->ulongAt(globalOffset));
        size = 4;
        break;
    case LONG_LONG_TYPE:
        value = QString().sprintf("%lli", binary->longLongAt(globalOffset));
        size = 8;
        break;
    case ULONG_LONG_TYPE:
        value = QString().sprintf("%llu", binary->ulongLongAt(globalOffset));
        size = 8;
        break;
    case FLOAT_TYPE:
        value = binary->floatAt(globalOffset);
        size = 4;
        break;
    case DOUBLE_TYPE:
        value = binary->doubleAt(globalOffset);
        size = 8;
        break;
    case LDOUBLE_TYPE:
        value = QString().sprintf("%lg", binary->ldoubleAt(globalOffset));
        size = 10;
        break;
    case STRING_TYPE: {
        value = binary->stringAt(globalOffset, size);
        break;
    }
    case HEX_TYPE: {
        value = binary->hexAt(globalOffset, size);
        break;
    }
    default: break;
    }
    updateOffset();
}

void JsonStoredData::writeDataValue(StructureByteArray *binary) {
    switch(type) {
    case CHAR_TYPE:
        binary->setCharAt(globalOffset, static_cast<char>(value.toInt()));
        break;
    case UCHAR_TYPE:
        binary->setUcharAt(globalOffset, static_cast<unsigned char>(value.toInt()));
        break;
    case SHORT_TYPE:
        binary->setShortAt(globalOffset, static_cast<short>(value.toInt()));
        break;
    case USHORT_TYPE:
        binary->setUshortAt(globalOffset, static_cast<unsigned short>(value.toInt()));
        break;
    case LONG_TYPE:
        binary->setLongAt(globalOffset, value.toString().toLong());
        break;
    case ULONG_TYPE:
        binary->setUlongAt(globalOffset, value.toString().toULong());
        break;
    case LONG_LONG_TYPE:
        binary->setLongLongAt(globalOffset, value.toString().toLongLong());
        break;
    case ULONG_LONG_TYPE:
        binary->setUlongLongAt(globalOffset, value.toString().toULongLong());
        break;
    case FLOAT_TYPE:
        binary->setFloatAt(globalOffset, value.toFloat());
        break;
    case DOUBLE_TYPE:
        binary->setDoubleAt(globalOffset, value.toDouble());
        break;
    case LDOUBLE_TYPE:
        //TODO fix precision
        binary->setLdoubleAt(globalOffset, value.toString().toDouble());
        break;
    case STRING_TYPE: {
        binary->setStringAt(globalOffset, value.toString());
        break;
    }
    case HEX_TYPE: {
        binary->setHexAt(globalOffset, value.toString());
        break;
    }
    default: break;
    }
}

QString JsonStoredData::ulongToHex(unsigned long value) {
    int size = value < 256 ? 2 : value < 65536 ? 4 : 8;
    return QString("0x%1").arg(value, size, 16, QChar('0'));
}

void JsonStoredData::updateOffset() {
    if (!resolved) {
        return;
    }
    if (type == OBJECT_TYPE && !fields.isEmpty()) {
        JsonStoredData *last = fields.at(0);
        for(JsonStoredData *data : fields) {
            if (data->offset > last->offset) {
                last = data;
            }
        }
        size = last->offset + last->size;
    }
    if (type == ARRAY_TYPE) {
        size = itemSize * count;
    }
    if (parent != nullptr) {
        if (arrayIndex >= 0) {
            offset = parent->itemSize * static_cast<unsigned long>(arrayIndex);
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
    if (parent != nullptr) {
        return parent->getRoot();
    }
    return this;
}

void JsonStoredData::readDataValues(StructureByteArray *binary) {
    readDataValue(binary);
    for(JsonStoredData *field : fields) {
        field->readDataValues(binary);
    }
}

unsigned long JsonStoredData::findValue(QString fullName, bool *ok) {
    QStringList lst = fullName.split(".");
    if (lst.isEmpty()) {
        *ok = false;
        return 0;
    }
    int ref = objectReferenceFields.indexOf(lst.last());
    if (ref != -1) {
        lst.removeLast();
    }
    JsonStoredData *current = getRoot();
    for (int i = 0; i < lst.size(); i++) {
        JsonStoredData *found = nullptr;
        for (JsonStoredData *field : current->fields) {
            if (field->name.compare(lst.at(i)) == 0) {
                found = field;
                break;
            }
        }
        current = found;
        if (found == nullptr) {
            break;
        }
    }
    if (current == nullptr) {
        *ok = false;
        return 0;
    }
    *ok = true;
    if (ref != -1) {
        switch(ref) {
        case OFFSET_FIELD: return current->getOffset();
        case SIZE_FIELD: return current->getSize();
        case GLOBAL_OFFSET_FIELD: return current->getGlobalOffset();
        case AFTER_FIELD: return current->getAfter();
        case COUNT_FIELD: return current->getCount();
        case ITEM_SIZE_FIELD: return current->getItemSize();
        default: break;
        }
    }
    return current->value.toUInt(ok);
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

bool JsonStoredData::resolveReferences(StructureByteArray *binary) {
    if (resolved) {
        if (binary != nullptr) {
            readDataValue(binary);
        }
        return true;
    }
    bool fail = false;
    updateReference(&size, &fail, sizeReference);
    updateReference(&offset, &fail, offsetReference);
    updateReference(&count, &fail, countReference);
    updateReference(&itemSize, &fail, itemSizeReference);
    if (fail) {
        qDebug() << "fail to resolve for " + fullName;
        return false;
    } else {
        qDebug() << "resolved for " + fullName;
        resolved = true;
        updateOffset();
        if (type == ARRAY_TYPE) {
            if (fields.size() == 1 && count != 1) {
                fields[0]->updateOffset();
                fields[0]->resolveReferences(binary);
                for (unsigned long i = 1; i < count; i++) {
                    auto field = fields[0]->createCopy();
                    field->setArrayIndex(static_cast<int>(i));
                    field->updateOffset();
                    field->resolveReferences(binary);
                    field->setName(QString("item[%1]").arg(i));
                    field->updateFullNames();
                    fields.append(field);
                }
            }
        }
        if (binary != nullptr) {
            readDataValue(binary);
        }
        return true;
    }
}

int JsonStoredData::resolveAllReferences(StructureByteArray *binary) {
    int count = 0;
    if (!resolved) {
        if (resolveReferences(binary)) {
            count++;
            for (auto field : fields) {
                count += field->resolveAllReferences(binary);
            }
        }
    } else {
        for (auto field : fields) {
            count += field->resolveAllReferences(binary);
        }
    }
    return count;
}

JsonStoredData *JsonStoredData::createCopy() {
    JsonStoredData *copied = new JsonStoredData(parent, offset, type, name, arrayIndex, size, count);
    copied->setGlobalOffset(globalOffset);
    copied->setFullName(fullName);
    copied->setAfter(after);
    copied->setValue(value);
    copied->setItemSize(itemSize);
    copied->setIsValid(isValid);
    copied->setCheckValue(checkValue);
    copied->setSizeReference(sizeReference);
    copied->setOffsetReference(offsetReference);
    copied->setCountReference(countReference);
    copied->setItemSizeReference(itemSizeReference);
    copied->setResolved(resolved);
    for (auto field : fields) {
        auto f = field->createCopy();
        f->setParent(copied);
        copied->appendField(f);
    }
    return copied;
}

void JsonStoredData::updateReference(unsigned long *value, bool *fail, QString reference) {
    if (!reference.isEmpty()) {
        bool ok = false;
        unsigned long v = findValue(reference, &ok);
        if (ok) {
            *value = v;
        } else {
            *fail = true;
        }
    }
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
