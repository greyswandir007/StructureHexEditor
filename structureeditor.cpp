#include "structureeditor.h"
#include "ui_structureeditor.h"
#include <QDebug>
#include <QTextCodec>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "hexeditor.h"
#include "components/bbytearray.h"
#include "components/jsonstoreddata.h"
#include <components/jsonstoreddatahelper.h>

StructureEditor::StructureEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StructureEditor) {
    ui->setupUi(this);
    utf8Codec = QTextCodec::codecForName("UTF-8");
}

StructureEditor::~StructureEditor() {
    delete ui;
}

void StructureEditor::parseJSONDocument(QString json) {
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(codec->fromUnicode(json), &error);
    QJsonObject jsonObject;
    if (jsonStoredData != nullptr) {
        JsonStoredDataHelper::freeData(jsonStoredData);
    }
    jsonStoredData = new JsonStoredData();

    ui->structureEdit->clear();
    if (error.error == QJsonParseError::NoError) {
        QStringList keys = document.object().keys();
        if (keys.contains("extesions") && document.object().value("extesions").isArray()) {
            jsonObject.insert("extesions", addExtensions(document.object().value("extesions").toArray()));
            keys.removeAt(keys.indexOf("extesions"));
        }
        if (keys.contains("signature") && document.object().value("signature").isString()) {
            jsonObject.insert("signature", checkSignature(document.object().value("signature").toString(), 0));
            keys.removeAt(keys.indexOf("signature"));
        }
        QJsonObject object = parseKeys(document.object(), 0, keys);
        for (QString key : object.keys()) {
            jsonObject.insert(key, object.value(key));
        }
        QJsonDocument parsedDocument(jsonObject);
        ui->structureEdit->setText(utf8Codec->toUnicode(parsedDocument.toJson(QJsonDocument::Indented)));
    } else {
        qDebug() << error.errorString();
    }
}

QString StructureEditor::formatJSONDocument(QString json) {
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(utf8Codec->fromUnicode(json), &error);
    if (error.error == QJsonParseError::NoError) {
        return utf8Codec->toUnicode(document.toJson(QJsonDocument::Indented));
    } else {
        qDebug() << error.errorString();
        return json;
    }
}

void StructureEditor::setHexEditor(HexEditor *hexEditor) {
    this->hexEditor = hexEditor;
}

QJsonArray StructureEditor::addExtensions(const QJsonArray &array) {
    QJsonArray jsonArray;
    for (int i = 0; i < array.size(); i++) {
        if (array.at(i).isString()) {
            jsonArray.append(QJsonValue(array.at(i).toString()));
        }
    }
    return jsonArray;
}

QJsonValue StructureEditor::checkSignature(QString signature, unsigned long baseOffset) {
    QString eq = "";
    if (signature.startsWith("0x") && hexEditor != nullptr) {
        int sigSize = (signature.size() - 2) / 2;
        QString hex = "0x" + hexEditor->getBinaryData()->hexAt(static_cast<int>(baseOffset), sigSize);
        eq = hex.compare(signature, Qt::CaseInsensitive) == 0 ? " (correct)" : " (incorrect)";
    }
    return QJsonValue(signature + eq);
}

QJsonObject StructureEditor::parseKeys(const QJsonObject &jsonObject, unsigned long baseOffset, const QStringList &keys) {
    QStringList keyList;
    if (keys.isEmpty()) {
        keyList = jsonObject.keys();
    } else {
        keyList = keys;
    }
    QJsonObject object;
    for (QString key : keyList) {
        auto value = jsonObject.value(key);
        if (value.isObject()) {
            QJsonValue receivedValue = parseObject(value.toObject(), baseOffset);
            if (!receivedValue.isNull() && !receivedValue.isUndefined()) {
                object.insert(key, receivedValue);
            }
        }
    }
    return object;
}

QJsonValue StructureEditor::parseObject(const QJsonObject &object, unsigned long baseOffset) {
    QJsonValue value;
    bool contatinsMandatoryKeys = true;
    for (QString field : objectMandatoryFields) {
        if (!object.keys().contains(field)) {
            contatinsMandatoryKeys = false;
        }
    }
    if (contatinsMandatoryKeys && hexEditor != nullptr) {
        unsigned long offset;
        int type = jsonTypes.indexOf(object.value("type").toString());
        if (object.value("offset").isString()) {
            QString offsetString = object.value("offset").toString();
            if (offsetString.startsWith("0x")) {
                bool status;
                offset = offsetString.mid(2).toULong(&status, 16);
                if (!status) {
                    return QJsonValue();
                }
            } else {
                bool status;
                offset = offsetString.toULong(&status, 10);
                if (!status) {
                    return QJsonValue();
                }
            }
        } else if (object.value("offset").isDouble()){
            offset = static_cast<unsigned long>(object.value("offset").toInt());
        } else {
            return QJsonValue();
        }
        offset += baseOffset;
        switch (type) {
        case SIGNATURE_TYPE: break;
        case CHAR_TYPE: return QJsonValue(static_cast<int>(hexEditor->getBinaryData()->at(static_cast<int>(offset))));
        case UCHAR_TYPE:
            return QJsonValue(static_cast<int>(hexEditor->getBinaryData()->ucharValueAt(static_cast<int>(offset))));
        case SHORT_TYPE: return QJsonValue(hexEditor->getBinaryData()->shortValueAt(static_cast<int>(offset)));
        case USHORT_TYPE: return QJsonValue(hexEditor->getBinaryData()->ushortValueAt(static_cast<int>(offset)));
        case LONG_TYPE:
            return QJsonValue(QString().sprintf("%i", hexEditor->getBinaryData()->longValueAt(static_cast<int>(offset))));
        case ULONG_TYPE:
            return QJsonValue(QString().sprintf("%u", hexEditor->getBinaryData()->ulongValueAt(static_cast<int>(offset))));
        case LONG_LONG_TYPE:
            return QJsonValue(QString().sprintf("%i", hexEditor->getBinaryData()->longLongValueAt(static_cast<int>(offset))));
        case ULONG_LONG_TYPE:
            return QJsonValue(QString().sprintf("%u", hexEditor->getBinaryData()->ulongLongValueAt(static_cast<int>(offset))));
        case FLOAT_TYPE:
            return QJsonValue(static_cast<double>(hexEditor->getBinaryData()->floatValueAt(static_cast<int>(offset))));
        case DOUBLE_TYPE:
            return QJsonValue(hexEditor->getBinaryData()->doubleValueAt(static_cast<int>(offset)));
        case LDOUBLE_TYPE:
            return QJsonValue(QString().sprintf("%lg", hexEditor->getBinaryData()->ldoubleValueAt(static_cast<int>(offset))));
        case STRING_TYPE: {
            if (!object.keys().contains("size") || !object.value("size").isDouble()) {
                return QJsonValue();
            }
            int size = object.value("size").toInt();
            return QJsonValue(hexEditor->getBinaryData()->stringAt(static_cast<int>(offset), size));
        }
        case HEX_TYPE: {
            if (!object.keys().contains("size") || !object.value("size").isDouble()) {
                return QJsonValue();
            }
            int size = object.value("size").toInt();
            return QJsonValue(hexEditor->getBinaryData()->hexAt(static_cast<int>(offset), size));
        }
        case ARRAY_TYPE: {
            if (!object.keys().contains("size") || !object.value("size").isDouble()
                    || !object.keys().contains("item") || !object.value("item").isObject()
                    || !object.keys().contains("itemSize") || !object.value("itemSize").isDouble()) {
                return QJsonValue();
            }
            int size = object.value("size").toInt();
            int is = object.value("itemSize").toInt();
            if (is < 0) {
                is = 0;
            }
            unsigned long itemSize = static_cast<unsigned long>(is);
            QJsonArray array;
            for (int i = 0; i < size; i++) {
                array.append(parseObject(object.value("item").toObject(), offset));
                offset += itemSize;
            }
            return QJsonValue(array);
        }
        case OBJECT_TYPE: {
            if (!object.keys().contains("fields") || !object.value("fields").isObject()) {
                return QJsonValue();
            }
            return QJsonValue(parseKeys(object.value("fields").toObject(), offset));
        }
        case FILE_TYPE: break;
        case IMAGE_TYPE: break;
        case IMAGE_SET_TYPE: break;
        case BINARY_TYPE: break;
        }
    }
    return QJsonValue ();
}
