#ifndef BBYTEARRAY_H
#define BBYTEARRAY_H

#include <QByteArray>
#include <QString>

class BByteArray : public QByteArray
{
public:
    BByteArray();
    BByteArray(QByteArray arr);

    long long longLongValueAt(int pos);
    long longValueAt(int pos);
    short shortValueAt(int pos);
    unsigned long long ulongLongValueAt(int pos);
    unsigned long ulongValueAt(int pos);
    unsigned short ushortValueAt(int pos);
    unsigned char ucharValueAt(int pos);
    float floatValueAt(int pos);
    double doubleValueAt(int pos);
    long double ldoubleValueAt(int pos);
    QString stringAt(int pos, int len);
    QString hexAt(int pos, int len);

    void setLongLongAt(int pos, long long value);
    void setLongAt(int pos, long value);
    void setShortAt(int pos, short value);
    void setUlongLongAt(int pos, unsigned long long value);
    void setUlongAt(int pos, unsigned long value);
    void setUShortAt(int pos, unsigned short value);
    void setUCharAt(int pos, unsigned char value);
    void setStringAt(int pos, QString str);

    void setLittleEndianFlag(bool flag);

private:
    bool littleEndianFlag = true;
};

#endif // BBYTEARRAY_H
