#ifndef JSONSTOREDDATA_H
#define JSONSTOREDDATA_H

#include "structurebytearray.h"

#include <QString>
#include <QList>

static const QList<QString> jsonTypes = {"none", "extensions", "signature", "char", "uchar", "short", "ushort", "long",
                                         "ulong", "longlong", "ulonglong", "float", "double", "ldouble", "string",
                                         "hex", "array", "object", "file", "image", "imageset", "binary"};

static const QList<QString> objectMandatoryFields = {"type", "offset"};

static const QList<QString> objectReferenceFields = {"offset", "size", "globalOffset", "after", "count", "itemSize"};

enum JsonTypes {
    NONE_TYPE, EXTENSIONS_TYPE, SIGNATURE_TYPE, CHAR_TYPE, UCHAR_TYPE, SHORT_TYPE, USHORT_TYPE, LONG_TYPE, ULONG_TYPE,
    LONG_LONG_TYPE, ULONG_LONG_TYPE, FLOAT_TYPE, DOUBLE_TYPE, LDOUBLE_TYPE, STRING_TYPE, HEX_TYPE,
    ARRAY_TYPE, OBJECT_TYPE, FILE_TYPE, IMAGE_TYPE, IMAGE_SET_TYPE, BINARY_TYPE
};

enum ReferenceFields {
    OFFSET_FIELD, SIZE_FIELD, GLOBAL_OFFSET_FIELD, AFTER_FIELD, COUNT_FIELD, ITEM_SIZE_FIELD
};

class JsonStoredData {

public:
    JsonStoredData(JsonStoredData *parent = nullptr);
    JsonStoredData(JsonStoredData *parent, int type, QString name);
    JsonStoredData(JsonStoredData *parent, unsigned long offset, int type, QString name, int arrayIndex = -1,
                   unsigned long size = 0, unsigned long count = 0);

    unsigned long getSize() const;
    void setSize(unsigned long value);
    unsigned long getOffset() const;
    void setOffset(unsigned long value);
    unsigned long getGlobalOffset() const;
    void setGlobalOffset(unsigned long value);
    unsigned long getAfter() const;
    void setAfter(unsigned long value);
    int getType() const;
    void setType(int value);
    QString getName() const;
    void setName(const QString &value);
    QString getFullName() const;
    void setFullName(const QString &value);
    QVariant getValue() const;
    void setValue(const QVariant &value);

    unsigned long getCount() const;
    void setCount(unsigned long value);
    unsigned long getItemSize() const;
    void setItemSize(unsigned long value);
    int getArrayIndex() const;
    void setArrayIndex(int value);
    bool getIsValid() const;
    void setIsValid(bool value);
    QString getCheckValue() const;
    void setCheckValue(const QString &value);

    QList<JsonStoredData *> getFields() const;
    void appendField(JsonStoredData *field);

    void freeData();
    QString toString(int identCount = 0);
    void readDataValue(StructureByteArray *binary);
    void writeDataValue(StructureByteArray *binary);

    QString ulongToHex(unsigned long value);

    void updateOffset();
    void reOrderFields();

    JsonStoredData *getParent() const;
    JsonStoredData *getRoot();

    void readDataValues(StructureByteArray *binary);

    unsigned long findValue(QString fullName, bool *ok);

    QString getSizeReference() const;
    void setSizeReference(const QString &value);

    QString getOffsetReference() const;
    void setOffsetReference(const QString &value);

    QString getCountReference() const;
    void setCountReference(const QString &value);

    QString getItemSizeReference() const;
    void setItemSizeReference(const QString &value);

    bool getResolved() const;
    void setResolved(bool value);

    bool resolveReferences(StructureByteArray *binary);
    int resolveAllReferences(StructureByteArray *binary);

    JsonStoredData *createCopy();

    void setParent(JsonStoredData *value);
    void updateFullNames();

private:
    void updateReference(unsigned long *value, bool *fail, QString reference);

    unsigned long size = 0;
    unsigned long offset = 0;
    unsigned long globalOffset = 0;
    unsigned long after = 0;
    int type = 0;
    QString name;
    QString fullName;
    QVariant value;

    JsonStoredData *parent = nullptr;

    unsigned long count = 0;
    unsigned long itemSize = 0;
    int arrayIndex = -1;
    bool isValid = true;
    QString checkValue;
    QList<JsonStoredData*> fields;

    QString sizeReference;
    QString offsetReference;
    QString countReference;
    QString itemSizeReference;

    bool resolved = true;

    struct {
        bool operator()(JsonStoredData * d1, JsonStoredData *d2) const {
            return d1->getGlobalOffset() < d2->getGlobalOffset();
        }
    } offsetCompare;
};

#endif // JSONSTOREDDATA_H
