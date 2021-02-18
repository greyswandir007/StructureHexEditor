#include "jsonstoreddata.h"
#include "jsonstoreddatahelper.h"
#include <QDebug>

JsonStoredDataHelper::JsonStoredDataHelper() {

}

QString JsonStoredDataHelper::dataToString(JsonStoredData *data, int identCount) {
    QString result;
        QString smallIdent = QString().fill(' ', identCount);
        result = smallIdent + "{";
        QString ident = "\n" + smallIdent + "    ";
        result += ident + QString("\"size\":%1").arg(data->getSize()) + ",";
        result += ident + "\"offset\":\"" + ulongToHex(data->getOffset()) + "\",";
        result += ident + "\"globalOffset\":\"" + ulongToHex(data->getGlobalOffset()) + "\",";
        result += ident + "\"after\":\"" + ulongToHex(data->getAfter()) + "\",";
        result += ident + "\"type\":\"" + jsonTypes.at(data->getType()) + "\",";
        result += ident + "\"name\":\"" + data->getName() + "\",";
        result += ident + "\"fullName\":\"" + data->getFullName() + "\",";
        result += ident + "\"value\":\"" + data->getValue().toString() + "\",";

        result += ident + QString("\"count\":%1").arg(data->getCount()) + ",";
        result += ident + "\"itemSize\":\"" + ulongToHex(data->getItemSize()) + "\",";
        result += ident + QString("\"arrayIndex\":%1").arg(data->getArrayIndex()) + ",";
        result += ident + "\"isValid\":\"" + (data->getIsValid() ? "true" : "false") + "\",";
        result += ident + "\"checkValue\":\"" + data->getCheckValue() + "\",";
        if (data->getFields().isEmpty()) {
            result += ident + "fields:[]";
        } else {
            result += ident + "fields:[\n";
            for (int i = 0; i < data->getFields().size(); i++) {
                result += dataToString(data->getFields().at(i), identCount + 8);
                if (i < data->getFields().size() - 1) {
                    result += ",\n";
                }
            }
            result += ident + "]";
        }
        result += "\n" + smallIdent + "}";
        return result;
}

void JsonStoredDataHelper::readDataValue(JsonStoredData *data, StructureByteArray *binary, JsonStoredData *root) {
    if (!data->getResolved()) {
        return;
    }
    unsigned int offset = data->getGlobalOffset();
    if (root != nullptr) {
        JsonStoredData *current = data->getParent();
        offset = data->getOffset();
        while (current != root && current != nullptr) {
            offset += current->getOffset();
            current = current->getParent();
        }
    }
    for (auto *field : data->getFields()) {
        JsonStoredDataHelper::readDataValue(field, binary);
    }
    switch(data->getType()) {
    case SIGNATURE_TYPE:
        if (data->getCheckValue().startsWith("0x")) {
            unsigned long sigSize = static_cast<unsigned long>((data->getCheckValue().size() - 2) / 2);
            QString hex = binary->hexAt(offset, sigSize);
            data->setValue(hex);
            data->setIsValid(data->getCheckValue().compare("0x" + hex, Qt::CaseInsensitive) == 0);
        } else {
            data->setIsValid(false);
        }
        break;
    case CHAR_TYPE:
        data->setValue(static_cast<int>(binary->at(static_cast<int>(offset))));
        break;
    case UCHAR_TYPE:
        data->setValue(binary->ucharAt(offset));
        break;
    case SHORT_TYPE:
        data->setValue(binary->shortAt(offset));
        break;
    case USHORT_TYPE:
        data->setValue(binary->ushortAt(offset));
        break;
    case LONG_TYPE:
        data->setValue(binary->longAt(offset));
        break;
    case ULONG_TYPE:
        data->setValue(binary->ulongAt(offset));
        break;
    case LONG_LONG_TYPE:
        data->setValue(QString().sprintf("%lli", binary->longLongAt(offset)));
        break;
    case ULONG_LONG_TYPE:
        data->setValue(QString().sprintf("%llu", binary->ulongLongAt(offset)));
        break;
    case FLOAT_TYPE:
        data->setValue(binary->floatAt(offset));
        break;
    case DOUBLE_TYPE:
        data->setValue(binary->doubleAt(offset));
        break;
    case LDOUBLE_TYPE:
        data->setValue(QString().sprintf("%lg", binary->ldoubleAt(offset)));
        break;
    case STRING_TYPE:
        data->setValue(binary->stringAt(offset, data->getSize()));
        break;
    case HEX_TYPE:
        data->setValue(binary->hexAt(offset, data->getSize()));
        break;
    case BINARY_TYPE:
        data->setValue(binary->mid(static_cast<int>(offset), static_cast<int>(data->getSize())));
        break;
    default: break;
    }
    data->updateOffset();
}

void JsonStoredDataHelper::writeDataValue(JsonStoredData *data, StructureByteArray *binary, JsonStoredData *root) {
    unsigned int offset = data->getGlobalOffset();
    if (root != nullptr) {
        JsonStoredData *current = data->getParent();
        offset = data->getOffset();
        while (current != root && current != nullptr) {
            offset += current->getOffset();
            current = current->getParent();
        }
    }
    switch(data->getType()) {
    case SIGNATURE_TYPE:
        binary->setHexAt(offset, data->getValue().toString());
        break;
    case CHAR_TYPE:
        binary->setCharAt(offset, static_cast<char>(data->getValue().toInt()));
        break;
    case UCHAR_TYPE:
        binary->setUcharAt(offset, static_cast<unsigned char>(data->getValue().toInt()));
        break;
    case SHORT_TYPE:
        binary->setShortAt(offset, static_cast<short>(data->getValue().toInt()));
        break;
    case USHORT_TYPE:
        binary->setUshortAt(offset, static_cast<unsigned short>(data->getValue().toInt()));
        break;
    case LONG_TYPE:
        binary->setLongAt(offset, data->getValue().toString().toLong());
        break;
    case ULONG_TYPE:
        binary->setUlongAt(offset, data->getValue().toString().toULong());
        break;
    case LONG_LONG_TYPE:
        binary->setLongLongAt(offset, data->getValue().toString().toLongLong());
        break;
    case ULONG_LONG_TYPE:
        binary->setUlongLongAt(offset, data->getValue().toString().toULongLong());
        break;
    case FLOAT_TYPE:
        binary->setFloatAt(offset, data->getValue().toFloat());
        break;
    case DOUBLE_TYPE:
        binary->setDoubleAt(offset, data->getValue().toDouble());
        break;
    case LDOUBLE_TYPE:
        //TODO fix precision
        binary->setLdoubleAt(offset, data->getValue().toString().toDouble());
        break;
    case STRING_TYPE:
        binary->setStringAt(offset, data->getValue().toString());
        break;
    case HEX_TYPE:
        binary->setHexAt(offset, data->getValue().toString());
        break;
    case BINARY_TYPE:
        binary->replace(static_cast<int>(offset), static_cast<int>(data->getSize()), data->getValue().toByteArray());
        break;
    default: break;
    }
}

JsonStoredData *JsonStoredDataHelper::createCopy(JsonStoredData *data) {
    JsonStoredData *copied = new JsonStoredData(data->getParent(), data->getOffset(), data->getType(), data->getName(),
                                                data->getArrayIndex(), data->getSize(), data->getCount());
    copied->setGlobalOffset(data->getGlobalOffset());
    copied->setFullName(data->getFullName());
    copied->setAfter(data->getAfter());
    copied->setValue(data->getValue());
    copied->setItemSize(data->getItemSize());
    copied->setIsValid(data->getIsValid());
    copied->setCheckValue(data->getCheckValue());
    copied->setSizeReference(data->getSizeReference());
    copied->setOffsetReference(data->getOffsetReference());
    copied->setCountReference(data->getCountReference());
    copied->setItemSizeReference(data->getItemSizeReference());
    copied->setDisplayName(data->getDisplayName());
    copied->setDisplayNameReference(data->getDisplayNameReference());
    copied->setResolved(data->getResolved());
    for (auto field : data->getFields()) {
        auto f = createCopy(field);
        f->setParent(copied);
        copied->appendField(f);
    }
    return copied;
}

QString JsonStoredDataHelper::ulongToHex(unsigned int value) {
    int size = value < 256 ? 2 : value < 65536 ? 4 : 8;
    return QString("0x%1").arg(value, size, 16, QChar('0'));
}

unsigned long JsonStoredDataHelper::findValue(JsonStoredData *data, QString fullName, bool *ok) {
    QStringList lst = fullName.split(".");
    if (lst.isEmpty()) {
        *ok = false;
        return 0;
    }
    int ref = objectReferenceFields.indexOf(lst.last());
    if (ref != -1) {
        lst.removeLast();
    }
    JsonStoredData *current = findDataByName(data, fullName);
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
    return current->getValue().toUInt(ok);
}

JsonStoredData *JsonStoredDataHelper::findDataByName(JsonStoredData *data, QString fullName) {
    JsonStoredData *current = data;
    if (fullName.startsWith(".")) {
        fullName = fullName.mid(1);
    } else {
        current = data->getRoot();
    }
    QStringList lst = fullName.split(".");
    if (lst.isEmpty()) {
        return nullptr;
    }
    int ref = objectReferenceFields.indexOf(lst.last());
    if (ref != -1) {
        lst.removeLast();
    }

    for (int i = 0; i < lst.size(); i++) {
        JsonStoredData *found = nullptr;
        for (JsonStoredData *field : current->getFields()) {
            QString searchName = lst.at(i);
            if (searchName.contains("[i]") && data->getArrayIndex() != -1) {
                searchName.replace("[i]", QString("[%1]").arg(data->getArrayIndex()));
            }
            if (field->getName().compare(searchName) == 0) {
                found = field;
                break;
            }
        }
        current = found;
        if (found == nullptr) {
            break;
        }
    }
    return current;
}

void JsonStoredDataHelper::objectToBinary(JsonStoredData *data, StructureByteArray *array, JsonStoredData *root) {
    if (root == nullptr) {
        root = data;
    }
    writeDataValue(data, array, root);
    for(auto field : data->getFields()) {
        objectToBinary(field, array, root);
    }
}

QList<JsonStoredData *> JsonStoredDataHelper::getBinaryList(JsonStoredData *data) {
    QList<JsonStoredData *> list;
    if (data->getType() == BINARY_TYPE) {
        list.append(data);
    }
    for (auto field : data->getFields()) {
        list.append(getBinaryList(field));
    }
    return list;
}

