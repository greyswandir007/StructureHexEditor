#include "structurebytearray.h"
#include <QTextCodec>
const char hexDigits[17] = "0123456789ABCDEF";

StructureByteArray::StructureByteArray() : QByteArray() {
}

StructureByteArray::StructureByteArray(QByteArray arr) : QByteArray(arr) {
}

long long StructureByteArray::longLongAt(unsigned long pos) {
    return static_cast<long long>(ulongLongAt(pos));
}

long StructureByteArray::longAt(unsigned long pos) {
    return static_cast<long>(ulongAt(pos));
}

short StructureByteArray::shortAt(unsigned long pos) {
    return static_cast<short>(ushortAt(pos));
}

unsigned long long StructureByteArray::ulongLongAt(unsigned long pos) {
    unsigned long long res;
    if (littleEndianFlag) {
        res = static_cast<unsigned long long>(ucharAt(pos));
        res += static_cast<unsigned long long>(ucharAt(pos + 1)) << 8;
        res += static_cast<unsigned long long>(ucharAt(pos + 2)) << 16;
        res += static_cast<unsigned long long>(ucharAt(pos + 3)) << 24;
        res += static_cast<unsigned long long>(ucharAt(pos + 4)) << 32;
        res += static_cast<unsigned long long>(ucharAt(pos + 5)) << 40;
        res += static_cast<unsigned long long>(ucharAt(pos + 6)) << 48;
        res += static_cast<unsigned long long>(ucharAt(pos + 7)) << 56;
    } else {
        res = static_cast<unsigned long long>(ucharAt(pos)) << 56;
        res += static_cast<unsigned long long>(ucharAt(pos + 1)) << 48;
        res += static_cast<unsigned long long>(ucharAt(pos + 2)) << 40;
        res += static_cast<unsigned long long>(ucharAt(pos + 3)) << 32;
        res += static_cast<unsigned long long>(ucharAt(pos + 4)) << 24;
        res += static_cast<unsigned long long>(ucharAt(pos + 5)) << 16;
        res += static_cast<unsigned long long>(ucharAt(pos + 6)) << 8;
        res += static_cast<unsigned long long>(ucharAt(pos + 7));
    }
    return res;
}

unsigned long StructureByteArray::ulongAt(unsigned long pos) {
    unsigned long res;
    if (littleEndianFlag) {
        res = static_cast<unsigned long>(ucharAt(pos));
        res += static_cast<unsigned long>(ucharAt(pos + 1)) << 8;
        res += static_cast<unsigned long>(ucharAt(pos + 2)) << 16;
        res += static_cast<unsigned long>(ucharAt(pos + 3)) << 24;
    } else {
        res = static_cast<unsigned long>(ucharAt(pos)) << 24;
        res += static_cast<unsigned long>(ucharAt(pos + 1)) << 16;
        res += static_cast<unsigned long>(ucharAt(pos + 2)) << 8;
        res += static_cast<unsigned long>(ucharAt(pos + 3));
    }
    return res;
}

unsigned short StructureByteArray::ushortAt(unsigned long pos) {
    unsigned short res;
    if (littleEndianFlag) {
        res = static_cast<unsigned short>(ucharAt(pos));
        res += static_cast<unsigned short>(ucharAt(pos + 1)) << 8;
    } else {
        res = static_cast<unsigned short>(static_cast<unsigned short>(ucharAt(pos)) << 8);
        res += static_cast<unsigned short>(ucharAt(pos + 1));
    }
    return res;

}

unsigned char StructureByteArray::ucharAt(unsigned long pos) {
    return pos >= static_cast<unsigned long>(size()) ? 0 : static_cast<unsigned char>(at(static_cast<int>(pos)));
}

float StructureByteArray::floatAt(unsigned long pos) {
    union {
        unsigned long ulong;
        float f;
    } converter;
    converter.ulong = ulongAt(pos);
    return converter.f;
}

double StructureByteArray::doubleAt(unsigned long pos) {
    union {
        unsigned long long ulongLong;
        double d;
    } converter;
    converter.ulongLong = ulongLongAt(pos);
    return converter.d;
}

long double StructureByteArray::ldoubleAt(unsigned long pos) {
    union {
        unsigned char uchar[10];
        long double ld;
    } converter;
    for (unsigned long i = 0; i < 10; i++) {
        converter.uchar[i] = ucharAt(pos + i);
    }
    return converter.ld;
}

QString StructureByteArray::stringAt(unsigned long pos, unsigned long len) {
    QString res;
    QTextCodec *codec = QTextCodec::codecForName("CP866");
    unsigned long i = pos;
    while (i < pos + len && i < static_cast<unsigned long>(size())) {
        char c = at(static_cast<int>(i));
        if (c != 0x0D && c != 0x0A) {
            res.append(codec->toUnicode(&c, 1));
        } else if (c == 0x0A) {
            res.append("\\n");
        } else {
            res.append("\\r");
        }
        i++;
    }
    return res;
}

QString StructureByteArray::hexAt(unsigned long pos, unsigned long len) {
    QString res;
    unsigned long i = pos;
    while (i < pos + len && static_cast<unsigned long>(size())) {
        unsigned char c = ucharAt(i);
        res.append(hexDigits[(c >> 4) & 0xF]);
        res.append(hexDigits[c & 0xF]);
        i++;
    }
    return res;
}

void StructureByteArray::setLongLongAt(unsigned long pos, long long value) {
    setUlongLongAt(pos, static_cast<unsigned long long>(value));
}

void StructureByteArray::setLongAt(unsigned long pos, long value) {
    setUlongAt(pos, static_cast<unsigned long>(value));
}

void StructureByteArray::setShortAt(unsigned long pos, short value) {
    setUshortAt(pos, static_cast<unsigned short>(value));
}

void StructureByteArray::setCharAt(unsigned long pos, char value) {
    setUcharAt(pos, static_cast<unsigned char>(value));
}

void StructureByteArray::setUlongLongAt(unsigned long pos, unsigned long long value) {
    if (littleEndianFlag) {
        setUcharAt(pos++, value & 0xFF);
        setUcharAt(pos++, (value >> 8) & 0xFF);
        setUcharAt(pos++, (value >> 16) & 0xFF);
        setUcharAt(pos++, (value >> 24) & 0xFF);
        setUcharAt(pos++, (value >> 32) & 0xFF);
        setUcharAt(pos++, (value >> 40) & 0xFF);
        setUcharAt(pos++, (value >> 48) & 0xFF);
        setUcharAt(pos++, (value >> 56)& 0xFF);
    } else {
        setUcharAt(pos++, (value >> 56)& 0xFF);
        setUcharAt(pos++, (value >> 48) & 0xFF);
        setUcharAt(pos++, (value >> 40) & 0xFF);
        setUcharAt(pos++, (value >> 32) & 0xFF);
        setUcharAt(pos++, (value >> 24)& 0xFF);
        setUcharAt(pos++, (value >> 16) & 0xFF);
        setUcharAt(pos++, (value >> 8) & 0xFF);
        setUcharAt(pos++, value & 0xFF);
    }
}

void StructureByteArray::setUlongAt(unsigned long pos, unsigned long value) {
    if (littleEndianFlag) {
        setUcharAt(pos++, value & 0xFF);
        setUcharAt(pos++, (value >> 8) & 0xFF);
        setUcharAt(pos++, (value >> 16) & 0xFF);
        setUcharAt(pos++, (value >> 24) & 0xFF);
    } else {
        setUcharAt(pos++, (value >> 24)& 0xFF);
        setUcharAt(pos++, (value >> 16) & 0xFF);
        setUcharAt(pos++, (value >> 8) & 0xFF);
        setUcharAt(pos++, value & 0xFF);
    }
}

void StructureByteArray::setUshortAt(unsigned long pos, unsigned short value) {
    if (littleEndianFlag) {
        setUcharAt(pos++, value & 0xFF);
        setUcharAt(pos++, (value >> 8) & 0xFF);
    } else {
        setUcharAt(pos++, (value >> 8) & 0xFF);
        setUcharAt(pos++, value & 0xFF);
    }
}

void StructureByteArray::setUcharAt(unsigned long pos, unsigned char value) {
    if (pos >= static_cast<unsigned long>(size())) {
        append(static_cast<char>(value));
    } else {
        data()[pos] = static_cast<char>(value);
    }
}

void StructureByteArray::setFloatAt(unsigned long pos, float value) {
    union {
        unsigned long ulong;
        float f;
    } converter;
    converter.f = value;
    setUlongAt(pos, converter.ulong);
}

void StructureByteArray::setDoubleAt(unsigned long pos, double value) {
    union {
        unsigned long long ulongLong;
        double d;
    } converter;
    converter.d = value;
    setUlongLongAt(pos, converter.ulongLong);
}

void StructureByteArray::setLdoubleAt(unsigned long pos, long double value) {
    union {
        unsigned char uchar[10];
        long double ld;
    } converter;
    converter.ld = value;
    for (unsigned long i = 0; i < 10; i++) {
        setUcharAt(pos + i, converter.uchar[i]);
    }
}

void StructureByteArray::setStringAt(unsigned long pos, QString str) {
    QTextCodec *codec = QTextCodec::codecForName("CP1251");
    QByteArray a = codec->fromUnicode(str);
    for(int i = 0; i < a.length(); i++) {
        if (pos +static_cast<unsigned long>(i) >= static_cast<unsigned long>(size())) {
            append(a.at(i));
        } else {
            data()[pos + static_cast<unsigned long>(i)] = a.at(i);
        }
    }
}

void StructureByteArray::setHexAt(unsigned long pos, QString hex) {
    QString res;
    unsigned long j = pos;
    int i = 0;
    while(i < hex.length()) {
        QString h = QString(hex.at(i));
        if (i + 1 < hex.length()) {
            h += hex.at(i + 1);
        }
        bool ok;
        char val = static_cast<char>(h.toInt(&ok, 16));
        if (ok) {
            if (j >= static_cast<unsigned long>(size())) {
                append(val);
            } else {
                data()[j] = val;
            }
        }
        j++;
        i += 2;
    }
}

void StructureByteArray::setLittleEndianFlag(bool flag) {
    littleEndianFlag = flag;
}
