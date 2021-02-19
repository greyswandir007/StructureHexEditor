#ifndef JSONSTOREDDATAHELPER_H
#define JSONSTOREDDATAHELPER_H

#include "structurebytearray.h"

#include <QString>

class JsonStoredData;

class JsonStoredDataHelper
{
public:
    JsonStoredDataHelper();

    static QString dataToString(JsonStoredData *data, int identCount = 0);
    static void readDataValue(JsonStoredData *data, StructureByteArray *binary, JsonStoredData *root = nullptr);
    static void writeDataValue(JsonStoredData *data, StructureByteArray *binary, JsonStoredData *root = nullptr);
    static JsonStoredData *createCopy(JsonStoredData *data);

    static QString ulongToHex(unsigned int value);

    static unsigned long findValue(JsonStoredData *data, QString fullName, bool *ok);
    static JsonStoredData *findDataByName(JsonStoredData *data, QString fullName);
    static void objectToBinary(JsonStoredData *data, StructureByteArray *array, JsonStoredData *root = nullptr);
    static QList<JsonStoredData *> getBinaryList(JsonStoredData *data);
};

#endif // JSONSTOREDDATAHELPER_H
