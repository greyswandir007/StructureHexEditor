#ifndef JSONSTOREDDATA_H
#define JSONSTOREDDATA_H

#include <QString>
#include <QList>

static const QList<QString> jsonTypes = {"extensions", "signature", "char", "uchar", "short", "ushort", "long", "ulong",
                                         "longlong", "ulonglong", "float", "double", "ldouble", "string", "hex",
                                         "array", "object", "file", "image", "imageset", "binary"};

static const QList<QString> objectMandatoryFields = {"type", "offset"};

enum JsonTypes {
    EXTENSIONS_TYPE, SIGNATURE_TYPE, CHAR_TYPE, UCHAR_TYPE, SHORT_TYPE, USHORT_TYPE, LONG_TYPE, ULONG_TYPE,
    LONG_LONG_TYPE, ULONG_LONG_TYPE, FLOAT_TYPE, DOUBLE_TYPE, LDOUBLE_TYPE, STRING_TYPE, HEX_TYPE,
    ARRAY_TYPE, OBJECT_TYPE, FILE_TYPE, IMAGE_TYPE, IMAGE_SET_TYPE, BINARY_TYPE
};

struct JsonStoredData {
    unsigned long size = 0;
    unsigned long offset = 0;
    unsigned long globalOffset = 0;
    unsigned long after = 0;
    int type = 0;
    unsigned long count = 0;
    QString name;
    QString fullName;
    QVariant value;
    JsonStoredData *parent = nullptr;
    QList<JsonStoredData*> fields;
};

#endif // JSONSTOREDDATA_H
