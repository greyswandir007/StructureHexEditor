#ifndef HEXMANAGER_H
#define HEXMANAGER_H

#include <QByteArray>
#include <QFile>
#include <QDebug>

class HexManager {
public:
    HexManager();
    ~HexManager();

    static QByteArray loadAsHex(QString file, bool *okay);
    static QStringList bin2Hex(QByteArray binary, quint8);
private:
    static QString  genHexLine(quint16 addr, QByteArray ba);
    static bool updateResult(QByteArray result, QString line, qint64 nbytes);
    static QByteArray getBytes(bool *check, QString data);
};

#endif // HEXMANAGER_H
