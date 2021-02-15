#ifndef BBYTEARRAY_H
#define BBYTEARRAY_H

#include <QByteArray>
#include <QString>

class StructureByteArray : public QByteArray
{
public:
    StructureByteArray();
    StructureByteArray(QByteArray arr);

    long long longLongAt(unsigned long pos);
    long longAt(unsigned long pos);
    short shortAt(unsigned long pos);
    unsigned long long ulongLongAt(unsigned long pos);
    unsigned long ulongAt(unsigned long pos);
    unsigned short ushortAt(unsigned long pos);
    unsigned char ucharAt(unsigned long pos);
    float floatAt(unsigned long pos);
    double doubleAt(unsigned long pos);
    long double ldoubleAt(unsigned long pos);
    QString stringAt(unsigned long pos, unsigned long len);
    QString hexAt(unsigned long pos, unsigned long len);

    void setLongLongAt(unsigned long pos, long long value);
    void setLongAt(unsigned long pos, long value);
    void setShortAt(unsigned long pos, short value);
    void setCharAt(unsigned long pos, char value);
    void setUlongLongAt(unsigned long pos, unsigned long long value);
    void setUlongAt(unsigned long pos, unsigned long value);
    void setUshortAt(unsigned long pos, unsigned short value);
    void setUcharAt(unsigned long pos, unsigned char value);
    void setFloatAt(unsigned long pos, float value);
    void setDoubleAt(unsigned long pos, double value);
    void setLdoubleAt(unsigned long pos, long double value);
    void setStringAt(unsigned long pos, QString str);
    void setHexAt(unsigned long pos, QString hex);

    void setLittleEndianFlag(bool flag);

private:
    bool littleEndianFlag = true;
};

#endif // BBYTEARRAY_H
