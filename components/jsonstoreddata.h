#ifndef JSONSTOREDDATA_H
#define JSONSTOREDDATA_H

#include "structurebytearray.h"

#include <QString>
#include <QList>

static const QList<QString> jsonTypes = {"none", "extensions", "signature", "char", "uchar", "short", "ushort", "long",
                                         "ulong", "longlong", "ulonglong", "float", "double", "ldouble", "string",
                                         "hex", "array", "object", "binary", "image", "imageset"};

static const QList<QString> objectMandatoryFields = {"type", "offset"};

static const QList<QString> objectReferenceFields = {"offset", "size", "globalOffset", "after", "count", "itemSize"};

static const unsigned long fixedTypesSize[] = {0, 0, 0, 1, 1, 2, 2, 4, 4, 8, 8, 4, 8, 10};

enum JsonTypes {
    NONE_TYPE, EXTENSIONS_TYPE, SIGNATURE_TYPE, CHAR_TYPE, UCHAR_TYPE, SHORT_TYPE, USHORT_TYPE, LONG_TYPE, ULONG_TYPE,
    LONG_LONG_TYPE, ULONG_LONG_TYPE, FLOAT_TYPE, DOUBLE_TYPE, LDOUBLE_TYPE, STRING_TYPE, HEX_TYPE,
    ARRAY_TYPE, OBJECT_TYPE, BINARY_TYPE, IMAGE_TYPE, IMAGE_SET_TYPE
};

enum ReferenceFields {
    OFFSET_FIELD, SIZE_FIELD, GLOBAL_OFFSET_FIELD, AFTER_FIELD, COUNT_FIELD, ITEM_SIZE_FIELD
};

class JsonStoredData {

public:
    JsonStoredData(JsonStoredData *parent = nullptr);
    JsonStoredData(JsonStoredData *parent, int type, QString name);
    JsonStoredData(JsonStoredData *parent, unsigned int offset, int type, QString name, int arrayIndex = -1,
                   unsigned int size = 0, unsigned int count = 0);

    unsigned int getSize() const;
    void setSize(unsigned int value);
    unsigned int getOffset() const;
    void setOffset(unsigned int value);
    unsigned int getGlobalOffset() const;
    void setGlobalOffset(unsigned int value);
    unsigned int getAfter() const;
    void setAfter(unsigned int value);
    int getType() const;
    void setType(int value);
    QString getName() const;
    void setName(const QString &value);
    QString getFullName() const;
    void setFullName(const QString &value);
    QVariant getValue() const;
    void setValue(const QVariant &value);

    unsigned int getCount() const;
    void setCount(unsigned int value);
    unsigned int getItemSize() const;
    void setItemSize(unsigned int value);
    int getArrayIndex() const;
    void setArrayIndex(int value);
    bool getIsValid() const;
    void setIsValid(bool value);
    QString getCheckValue() const;
    void setCheckValue(const QString &value);

    QList<JsonStoredData *> getFields() const;
    void appendField(JsonStoredData *field);

    void freeData();

    void updateOffset();
    void reOrderFields();

    JsonStoredData *getParent() const;
    JsonStoredData *getRoot();

    void readDataValues(StructureByteArray *binary);

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

    bool resolveReferences(StructureByteArray *binary, bool *update = nullptr);
    int resolveAllReferences(StructureByteArray *binary);

    void setParent(JsonStoredData *value);
    void updateFullNames();

private:
    void updateReference(unsigned int *value, bool *fail, bool *update, QString reference);

    unsigned int size = 0;
    unsigned int offset = 0;
    unsigned int globalOffset = 0;
    unsigned int after = 0;
    int type = 0;
    QString name;
    QString fullName;
    QVariant value;

    JsonStoredData *parent = nullptr;

    unsigned int count = 0;
    unsigned int itemSize = 0;
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
