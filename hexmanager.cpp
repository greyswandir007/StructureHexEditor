#include "hexmanager.h"

/*
 Intel hex format:

Data Record: ':nnaaaattdddddddddddddddddddddddddddddd...cc'
Where:
: = Indicates start of data record
n = Count of number of bytes in record. (in ASCII/HEX)
a = Load address of data record. (in ASCII/HEX)
t = Record type (00=Data, 01=End of file)
d = Actual data bytes in record (in ASCII/HEX)
c = Checksum of count, address, and data. (in ASCII/HEX)

Note1: Checksum is computed as two's complement of eight bit sum of all values fron 'nn' to end of data.

Note2: End of file record contains count of 00.
*/

HexManager::HexManager() {

}

HexManager::~HexManager() {

}

QByteArray HexManager::loadAsHex(QString file, bool *okay) {
        QByteArray result;
        QFile input(file);

        if (okay) {
            *okay = false;
        }
        if (input.exists()) {
            QString line;
            qint64 nbytes;

            qDebug() << " HEX import of " << file;

            if (input.open(QIODevice::ReadOnly)) {
                do {
                    line = input.readLine(1024);
                    // Отрезать \n\r если таковые есть в строке
                    int rn_index = line.indexOf('\n');
                    if (rn_index != -1) {
                        line = line.mid(0, rn_index);
                    }
                    rn_index = line.indexOf('\r');
                    if (rn_index != -1) {
                        line = line.mid(0, rn_index);
                    }
                    nbytes = line.length();
                    if (!updateResult(result, line, nbytes)) {
                        break;
                    }
                } while(nbytes);
            }
            if (result.size()) {
                *okay = true;
            }
        }
        return result;
}

bool HexManager::updateResult(QByteArray result, QString line, qint64 nbytes) {
    if (nbytes) {
        if (line.at(0) != ':') {
            result.clear();
            return false;
        } else {
            bool check;
            quint8 calcTheSum = 0;

            QString nn_count = line.mid(1, 2);
            QString aaaa_address = line.mid(3, 4);
            QString tt_recordType = line.mid(7, 2);
            QString dd_data = line.mid(9, nbytes - 11);
            QString cc_checksum = line.mid(nbytes - 2, 2);
            QString calc_checksum = nn_count + aaaa_address + tt_recordType + dd_data;

            // Нечетная длина строки с данными?
            if (dd_data.length() % 2) {
                result.clear();
                return false;
            }

            /*quint64 count = nn_count.toUInt(&check, 16);
            if (!check) {
                result.clear();
                return false;
            }*/

            quint64 address = aaaa_address.toUInt(&check, 16);
            if (!check) {
                result.clear();
                return false;
            }

            quint64 recordType = tt_recordType.toUInt(&check, 16);
            if (!check) {
                result.clear();
                return false;
            }

            quint8 checksum = static_cast<quint8>(cc_checksum.toUInt(&check, 16));
            if (!check) {
                result.clear();
                return false;
            }

            for (int i = 0; i < calc_checksum.length() / 2; i++) {
                QString s_byte = calc_checksum.mid(i * 2, 2);
                calcTheSum += static_cast<quint8>(s_byte.toUInt(&check, 16));
            }

            quint8 validation = calcTheSum + checksum;

            // Не ноль?
            if (validation) {
                result.clear();
                return false;
            }

            // Конец файла?
            if (recordType != 00) {
                if (recordType == 01) {
                    qDebug() << "end of HEX file";
                    return false;
                }
                qDebug() << "recordType " << recordType;
                return true;
            }

            QByteArray bytesLine = getBytes(&check, dd_data);
            if (!check) {
                result.clear();
                return false;
            }

            // Автоматический resize QByteArray  заполняет его не нулями, а 0x20, что неприемлемо
            if (static_cast<unsigned int>(result.size()) < address) {
                QByteArray  hole;
                hole.resize(static_cast<int>(address) - result.size());
                hole.fill(0x00);
                result.append(hole);
            }

            result.remove(static_cast<int>(address), bytesLine.size());
            result.insert(static_cast<int>(address), bytesLine);
        }
    }
    return true;
}

QByteArray HexManager::getBytes(bool *check, QString data) {
    QByteArray bytesLine;
    for (int i = 0; i < data.length() / 2; i++) {
        QString sb = data.mid(i * 2, 2);
        quint8 byte = static_cast<quint8>(sb.toUInt(check, 16));
        if (!check) {
            break;
        }
        bytesLine.append(static_cast<char>(byte));
    }
    return bytesLine;
}

QStringList HexManager::bin2Hex(QByteArray binary, quint8 cols) {
        quint64 bytes = static_cast<quint64>(binary.size());
        quint16 addr = 0;
        QStringList result;

        // Основная часть
        for (quint64 i = 0; i < bytes / cols; i ++) {
            QByteArray  ba_line = binary.mid(addr, cols);
            result.append(genHexLine(addr, ba_line) + "\n");
            addr += cols;
        }

        // И то, что осталось после деления нацело по числу столбцов cols
        if (quint8 tail_bytes = (bytes % cols)) {
            QByteArray  ba_tail = binary.mid(addr, tail_bytes);
            result.append(genHexLine(addr, ba_tail) + "\n");
        }

        // И строка конца файла
        result.append(":00000001FF\n");
        return result;
}

QString  HexManager::genHexLine(quint16 addr, QByteArray ba) {
    union {
        quint16 addr_16;
        quint8  addr_byte[2];
    } addr_split;
    quint8 bytes = static_cast<quint8>(ba.size());
    addr_split.addr_16 = addr;
    quint8 csum = 0;
    QString data = "";

    for (int j = 0; j < bytes; j ++) {
        data += QString().sprintf("%02X", static_cast<quint8>(ba.at(j)));
        csum += static_cast<quint8>(ba.at(j));
    }

    csum += bytes + addr_split.addr_byte[0] + addr_split.addr_byte[1];
    csum = -csum;
    return ":" + QString().sprintf("%02X", bytes) + QString().sprintf("%04X", addr) + "00"+ data
            + QString().sprintf("%02X", csum);
}
