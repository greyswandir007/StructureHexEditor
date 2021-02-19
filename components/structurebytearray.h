#ifndef BBYTEARRAY_H
#define BBYTEARRAY_H

#include <QByteArray>
#include <QString>

class StructureByteArray : public QByteArray
{
public:
    StructureByteArray();
    StructureByteArray(QByteArray arr);

    long long longLongAt(unsigned int pos);
    int longAt(unsigned int pos);
    short shortAt(unsigned int pos);
    unsigned long long ulongLongAt(unsigned int pos);
    unsigned int ulongAt(unsigned int pos);
    unsigned short ushortAt(unsigned int pos);
    unsigned char ucharAt(unsigned int pos);
    float floatAt(unsigned int pos);
    double doubleAt(unsigned int pos);
    long double ldoubleAt(unsigned int pos);
    unsigned int rgbAt(unsigned int pos);
    unsigned int bgrAt(unsigned int pos);
    QString stringAt(unsigned int pos, unsigned int len);
    QString hexAt(unsigned int pos, unsigned int len);

    void setLongLongAt(unsigned int pos, long long value);
    void setLongAt(unsigned int pos, int value);
    void setShortAt(unsigned int pos, short value);
    void setCharAt(unsigned int pos, char value);
    void setUlongLongAt(unsigned int pos, unsigned long long value);
    void setUlongAt(unsigned int pos, unsigned int value);
    void setUshortAt(unsigned int pos, unsigned short value);
    void setUcharAt(unsigned int pos, unsigned char value);
    void setFloatAt(unsigned int pos, float value);
    void setDoubleAt(unsigned int pos, double value);
    void setLdoubleAt(unsigned int pos, long double value);
    void setRgbAt(unsigned int pos, unsigned int value);
    void setBgrAt(unsigned int pos, unsigned int value);
    void setStringAt(unsigned int pos, QString str);
    void setHexAt(unsigned int pos, QString hex);

    void setLittleEndianFlag(bool flag);

private:
    bool littleEndianFlag = true;
};

#endif // BBYTEARRAY_H
