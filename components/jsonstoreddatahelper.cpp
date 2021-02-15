#include "jsonstoreddatahelper.h"

JsonStoredDataHelper::JsonStoredDataHelper() {

}

void JsonStoredDataHelper::freeData(JsonStoredData *jsonStoredData) {
    for (JsonStoredData *data : jsonStoredData->fields) {
        freeData(data);
    }
    delete jsonStoredData;
}

QString JsonStoredDataHelper::printData(JsonStoredData *jsonStoredData, int identCount) {
    QString result;
    QString ident = "\n" + QString().fill(' ', identCount);
    result = "{";
    result += ident + QString("\"size\":%1").arg(jsonStoredData->size) + ",";
    result += ident + "\"offset\":\"" + ulongToHex(jsonStoredData->offset) + "\",";
    result += ident + "\"globalOffset\":\"" + ulongToHex(jsonStoredData->globalOffset) + "\",";
    result += ident + "\"after\":\"" + ulongToHex(jsonStoredData->after) + "\",";
    result += ident + "\"type\":\"" + jsonTypes.at(jsonStoredData->type) + "\",";
    result += ident + QString("\"count\":%1").arg(jsonStoredData->count) + ",";
    result += ident + "\"name\":\"" + jsonStoredData->name + "\",";
    result += ident + "\"fullName\":\"" + jsonStoredData->fullName + "\",";
    result += ident + "\"value\":\"" + jsonStoredData->value.toString() + "\",";
    result += ident + "fields:[";
    if (!jsonStoredData->fields.isEmpty()) {
        result += "\n";
        for (int i = 0; i < jsonStoredData->fields.size(); i++) {
            JsonStoredData *data = jsonStoredData->fields.at(i);
            result += printData(data, identCount + 4);
            if (i < jsonStoredData->fields.size() - 1) {
                result += ",";
            }
            result += "\n";
        }
    }
    result += ident + "]\n}";
    return result;
}

QString JsonStoredDataHelper::ulongToHex(unsigned long value) {
    int size = value < 256 ? 2 : value < 65536 ? 4 : 8;
    return QString("0x%1").arg(value, size, 16, QChar('0'));
}
