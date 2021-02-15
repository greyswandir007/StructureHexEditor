#ifndef JSONSTOREDDATAHELPER_H
#define JSONSTOREDDATAHELPER_H

#include "jsonstoreddata.h"



class JsonStoredDataHelper {
public:
    JsonStoredDataHelper();

    static void freeData(JsonStoredData *jsonStoredData);
    static QString printData(JsonStoredData *jsonStoredData, int identCount = 0);

    static QString ulongToHex(unsigned long value);
};

#endif // JSONSTOREDDATAHELPER_H
