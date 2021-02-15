#include "bbytearray.h"
#include <QTextCodec>
const char hexDigits[17] = "0123456789ABCDEF";

BByteArray::BByteArray() : QByteArray() {
}

BByteArray::BByteArray(QByteArray arr) : QByteArray(arr) {
}

long long BByteArray::longLongValueAt(int pos) {
    return static_cast<long long>(ulongLongValueAt(pos));
}

long BByteArray::longValueAt(int pos) {
    return static_cast<long>(ulongValueAt(pos));
}

short BByteArray::shortValueAt(int pos) {
    return static_cast<short>(ushortValueAt(pos));
}

unsigned long long BByteArray::ulongLongValueAt(int pos) {
    unsigned long long res;
    if (littleEndianFlag) {
        res = static_cast<unsigned long long>(ucharValueAt(pos));
        res += static_cast<unsigned long long>(ucharValueAt(pos + 1)) << 8;
        res += static_cast<unsigned long long>(ucharValueAt(pos + 2)) << 16;
        res += static_cast<unsigned long long>(ucharValueAt(pos + 3)) << 24;
        res += static_cast<unsigned long long>(ucharValueAt(pos + 4)) << 32;
        res += static_cast<unsigned long long>(ucharValueAt(pos + 5)) << 40;
        res += static_cast<unsigned long long>(ucharValueAt(pos + 6)) << 48;
        res += static_cast<unsigned long long>(ucharValueAt(pos + 7)) << 56;
    } else {
        res = static_cast<unsigned long long>(ucharValueAt(pos)) << 56;
        res += static_cast<unsigned long long>(ucharValueAt(pos + 1)) << 48;
        res += static_cast<unsigned long long>(ucharValueAt(pos + 2)) << 40;
        res += static_cast<unsigned long long>(ucharValueAt(pos + 3)) << 32;
        res += static_cast<unsigned long long>(ucharValueAt(pos + 4)) << 24;
        res += static_cast<unsigned long long>(ucharValueAt(pos + 5)) << 16;
        res += static_cast<unsigned long long>(ucharValueAt(pos + 6)) << 8;
        res += static_cast<unsigned long long>(ucharValueAt(pos + 7));
    }
    return res;
}

unsigned long BByteArray::ulongValueAt(int pos) {
    unsigned long res;
    if (littleEndianFlag) {
        res = static_cast<unsigned long>(ucharValueAt(pos));
        res += static_cast<unsigned long>(ucharValueAt(pos + 1)) << 8;
        res += static_cast<unsigned long>(ucharValueAt(pos + 2)) << 16;
        res += static_cast<unsigned long>(ucharValueAt(pos + 3)) << 24;
    } else {
        res = static_cast<unsigned long>(ucharValueAt(pos)) << 24;
        res += static_cast<unsigned long>(ucharValueAt(pos + 1)) << 16;
        res += static_cast<unsigned long>(ucharValueAt(pos + 2)) << 8;
        res += static_cast<unsigned long>(ucharValueAt(pos + 3));
    }
    return res;
}

unsigned short BByteArray::ushortValueAt(int pos) {
    unsigned short res;
    if (littleEndianFlag) {
        res = static_cast<unsigned short>(ucharValueAt(pos));
        res += static_cast<unsigned short>(ucharValueAt(pos + 1)) << 8;
    } else {
        res = static_cast<unsigned short>(static_cast<unsigned short>(ucharValueAt(pos)) << 8);
        res += static_cast<unsigned short>(ucharValueAt(pos + 1));
    }
    return res;

}

unsigned char BByteArray::ucharValueAt(int pos) {
    return pos >= size() ? 0 : static_cast<unsigned char>(at(pos));
}

float BByteArray::floatValueAt(int pos) {
    union {
        unsigned long ulong;
        float f;
    } converter;
    converter.ulong = ulongValueAt(pos);
    return converter.f;
}

double BByteArray::doubleValueAt(int pos) {
    union {
        unsigned long long ulongLong;
        double d;
    } converter;
    converter.ulongLong = ulongLongValueAt(pos);
    return converter.d;
}

long double BByteArray::ldoubleValueAt(int pos) {
    union {
        unsigned char ushort[10];
        long double ld;
    } converter;
    for (int i = 0; i < 10; i++) {
        converter.ushort[i] = ucharValueAt(pos + i);
    }
    return converter.ld;
}

QString BByteArray::stringAt(int pos, int len) {
    QString res;
    QTextCodec *codec = QTextCodec::codecForName("CP866");
    int i = pos;
    while (i < pos + len && i < size()) {
        char c = at(i);
        if (c != 0x0D && c != 0x0A) {
            res.append(codec->toUnicode(&c,1));
        } else if (c == 0x0A) {
            res.append("\\n");
        } else {
            res.append("\\r");
        }
        i++;
    }
    return res;
}

QString BByteArray::hexAt(int pos, int len) {
    QString res;
    int i = pos;
    while (i < pos + len && i < size()) {
        unsigned char c = ucharValueAt(i);
        res.append(hexDigits[(c >> 4) & 0xF]);
        res.append(hexDigits[c & 0xF]);
        i++;
    }
    return res;
}

void BByteArray::setLongLongAt(int pos, long long value) {
    setUlongLongAt(pos, static_cast<unsigned long long>(value));
}

void BByteArray::setLongAt(int pos, long value) {
    setUlongAt(pos, static_cast<unsigned long>(value));
}

void BByteArray::setShortAt(int pos, short value) {
    setUShortAt(pos, static_cast<unsigned short>(value));
}

void BByteArray::setUlongLongAt(int pos, unsigned long long value) {
    if (littleEndianFlag) {
        setUCharAt(pos++, value & 0xFF);
        setUCharAt(pos++, (value >> 8) & 0xFF);
        setUCharAt(pos++, (value >> 16) & 0xFF);
        setUCharAt(pos++, (value >> 24) & 0xFF);
        setUCharAt(pos++, (value >> 32) & 0xFF);
        setUCharAt(pos++, (value >> 40) & 0xFF);
        setUCharAt(pos++, (value >> 48) & 0xFF);
        setUCharAt(pos++, (value >> 56)& 0xFF);
    } else {
        setUCharAt(pos++, (value >> 56)& 0xFF);
        setUCharAt(pos++, (value >> 48) & 0xFF);
        setUCharAt(pos++, (value >> 40) & 0xFF);
        setUCharAt(pos++, (value >> 32) & 0xFF);
        setUCharAt(pos++, (value >> 24)& 0xFF);
        setUCharAt(pos++, (value >> 16) & 0xFF);
        setUCharAt(pos++, (value >> 8) & 0xFF);
        setUCharAt(pos++, value & 0xFF);
    }
}

void BByteArray::setUlongAt(int pos, unsigned long value) {
    if (littleEndianFlag) {
        setUCharAt(pos++, value & 0xFF);
        setUCharAt(pos++, (value >> 8) & 0xFF);
        setUCharAt(pos++, (value >> 16) & 0xFF);
        setUCharAt(pos++, (value >> 24) & 0xFF);
    } else {
        setUCharAt(pos++, (value >> 24)& 0xFF);
        setUCharAt(pos++, (value >> 16) & 0xFF);
        setUCharAt(pos++, (value >> 8) & 0xFF);
        setUCharAt(pos++, value & 0xFF);
    }
}

void BByteArray::setUShortAt(int pos, unsigned short value) {
    if (littleEndianFlag) {
        setUCharAt(pos++, value & 0xFF);
        setUCharAt(pos++, (value >> 8) & 0xFF);
    } else {
        setUCharAt(pos++, (value >> 8) & 0xFF);
        setUCharAt(pos++, value & 0xFF);
    }
}

void BByteArray::setUCharAt(int pos, unsigned char value) {
    if (pos >= count()) {
        append(static_cast<char>(value));
    } else {
        data()[pos] = static_cast<char>(value);
    }
}

void BByteArray::setStringAt(int pos, QString str) {
    QTextCodec *codec = QTextCodec::codecForName("CP866");
    QByteArray a = codec->fromUnicode(str);
    for(int i = 0; i < a.length(); i++) {
        if (pos >= count()) {
            append(a.at(i));
        } else {
            data()[pos] = a.at(i);
        }
    }
}

void BByteArray::setLittleEndianFlag(bool flag) {
    littleEndianFlag = flag;
}
