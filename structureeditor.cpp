#include "structureeditor.h"
#include "ui_structureeditor.h"
#include <QDebug>
#include <QTextCodec>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "hexeditor.h"
#include "components/structurebytearray.h"
#include "components/jsonstoreddata.h"
#include <components/jsonstoreddatahelper.h>
#include <QFileDialog>
#include <QMenu>

StructureEditor::StructureEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StructureEditor) {
    ui->setupUi(this);
    utf8Codec = QTextCodec::codecForName("UTF-8");
    model = new JsonTreeModel();
    ui->structureView->setModel(model);
}

StructureEditor::~StructureEditor() {
    delete ui;
    if (jsonStoredData != nullptr) {
        jsonStoredData->freeData();
        delete jsonStoredData;
    }
    delete model;
}

void StructureEditor::parseJSONDocument(QString json) {
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(codec->fromUnicode(json), &error);
    QJsonObject jsonObject;
    if (jsonStoredData != nullptr) {
        jsonStoredData->freeData();
        delete jsonStoredData;
    }
    jsonStoredData = new JsonStoredData();
    if (error.error == QJsonParseError::NoError) {
        QStringList keys = document.object().keys();
        if (keys.contains("extesions") && document.object().value("extesions").isArray()) {
            JsonStoredData *data = new JsonStoredData(jsonStoredData, EXTENSIONS_TYPE, "extensions");
            data->setValue(addExtensions(document.object().value("extesions").toArray()));
            keys.removeAt(keys.indexOf("extesions"));
            jsonStoredData->appendField(data);
        }
        if (keys.contains("signature") && document.object().value("signature").isString()) {
            JsonStoredData *data = new JsonStoredData(jsonStoredData, SIGNATURE_TYPE, "signature");
            data->setCheckValue(document.object().value("signature").toString());
            JsonStoredDataHelper::readDataValue(data, hexEditor->getBinaryData());
            jsonStoredData->appendField(data);
            keys.removeAt(keys.indexOf("signature"));
        }
        parseObject(document.object(), jsonStoredData, keys);
        while (jsonStoredData->resolveAllReferences(hexEditor->getBinaryData()) > 0);
        jsonStoredData->reOrderFields();
        model->setRoot(jsonStoredData);
    } else {
        qDebug() << error.errorString();
    }
}

QString StructureEditor::formatJSONDocument(QString json) {
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(utf8Codec->fromUnicode(json), &error);
    if (error.error == QJsonParseError::NoError) {
        return utf8Codec->toUnicode(document.toJson(QJsonDocument::Indented));
    }
    qDebug() << error.errorString();
    return json;
}

void StructureEditor::setHexEditor(HexEditor *hexEditor) {
    this->hexEditor = hexEditor;
}

void StructureEditor::clearStructure() {
    if (jsonStoredData != nullptr) {
        jsonStoredData->freeData();
        delete jsonStoredData;
        jsonStoredData = nullptr;
    }
    model->setRoot(jsonStoredData);
}

QStringList StructureEditor::getBinaryList() {
    return JsonStoredDataHelper::getBinaryList(jsonStoredData);
}

JsonStoredData *StructureEditor::getStoredDataByName(QString name) {
    return JsonStoredDataHelper::findDataByName(jsonStoredData, name);
}

QStringList StructureEditor::addExtensions(const QJsonArray &array) {
    QStringList extensions;
    for (int i = 0; i < array.size(); i++) {
        if (array.at(i).isString()) {
            extensions.append(array.at(i).toString());
        }
    }
    return extensions;
}

void StructureEditor::parseObject(const QJsonObject &object, JsonStoredData *data, const QStringList &keys) {
    QStringList keyList = keys.isEmpty() ? object.keys() : keys;
    for (QString key : keyList) {
        QJsonValue value = object.value(key);
        if (value.isObject()) {
            parseValue(value.toObject(), key, data);
        }
    }
    data->reOrderFields();
    data->updateOffset();
}

void StructureEditor::parseValue(const QJsonObject &object, QString name, JsonStoredData *data, int arrayIndex) {
    JsonStoredData *field = nullptr;
    bool contatinsMandatoryKeys = true;
    for (QString field : objectMandatoryFields) {
        if (!object.keys().contains(field)) {
            contatinsMandatoryKeys = false;
        }
    }
    if (contatinsMandatoryKeys && hexEditor != nullptr) {
        unsigned long offset = 0;
        QString offsetReference;
        bool resolved = true;
        int type = jsonTypes.indexOf(object.value("type").toString());
        if (object.value("offset").isString()) {
            QString offsetString = object.value("offset").toString();
            if (offsetString.startsWith("0x")) {
                bool status;
                offset = offsetString.mid(2).toULong(&status, 16);
                if (!status) {
                    return;
                }
            } else {
                bool status;
                offset = offsetString.toULong(&status, 10);
                if (!status) {
                    offsetReference = offsetString;
                    resolved = false;
                }
            }
        } else if (object.value("offset").isDouble()){
            offset = static_cast<unsigned long>(object.value("offset").toInt());
        } else {
            return;
        }
        if (type >= CHAR_TYPE && type <= LDOUBLE_TYPE) {
            field = new JsonStoredData(data, offset, type, name, arrayIndex);
            if (resolved) {
                JsonStoredDataHelper::readDataValue(field, hexEditor->getBinaryData());
            } else {
                field->setOffsetReference(offsetReference);
                field->resolveReferences(hexEditor->getBinaryData());
            }
        } else if (type == STRING_TYPE || type == HEX_TYPE || type == BINARY_TYPE) {
            if (object.keys().contains("size")) {
                if (object.value("size").isDouble()) {
                    unsigned long size = static_cast<unsigned long>(object.value("size").toInt());
                    field = new JsonStoredData(data, offset, type, name, arrayIndex, size);
                    if (resolved) {
                        JsonStoredDataHelper::readDataValue(field, hexEditor->getBinaryData());
                    } else {
                        field->setOffsetReference(offsetReference);
                        field->resolveReferences(hexEditor->getBinaryData());
                    }
                } else if (object.value("size").isString()) {
                    field = new JsonStoredData(data, offset, type, name, arrayIndex);
                    QString reference = object.value("size").toString();
                    field->setSizeReference(reference);
                    if (!resolved) {
                        field->setOffsetReference(offsetReference);
                    }
                    field->resolveReferences(hexEditor->getBinaryData());
                }
            }
        } else {
            switch (type) {
            case SIGNATURE_TYPE:
                if (object.keys().contains("size") && object.keys().contains("signature")
                        && object.value("signature").isString()) {
                    if (object.value("size").isDouble()) {
                        unsigned long size = static_cast<unsigned long>(object.value("size").toInt());
                        field = new JsonStoredData(data, offset, type, name, arrayIndex, size);
                        field->setCheckValue(object.value("signature").toString());
                        if (resolved) {
                            JsonStoredDataHelper::readDataValue(field, hexEditor->getBinaryData());
                        } else {
                            field->setOffsetReference(offsetReference);
                            field->resolveReferences(hexEditor->getBinaryData());
                        }
                    } else if (object.value("size").isString()) {
                        field = new JsonStoredData(data, offset, type, name, arrayIndex);
                        field->setCheckValue(object.value("signature").toString());
                        QString reference = object.value("size").toString();
                        field->setSizeReference(reference);
                        if (!resolved) {
                            field->setOffsetReference(offsetReference);
                        }
                        field->resolveReferences(hexEditor->getBinaryData());
                    }
                }
                break;
            case ARRAY_TYPE:
                if (object.keys().contains("size") && object.keys().contains("item") && object.value("item").isObject()) {
                    field = new JsonStoredData(data, offset, type, name, arrayIndex);
                    if (!resolved) {
                        field->setOffsetReference(offsetReference);
                    }
                    if (object.value("size").isDouble()) {
                        unsigned long size = static_cast<unsigned long>(object.value("size").toInt());
                        field->setCount(size);
                    } else if (object.value("size").isString()) {
                        QString reference = object.value("size").toString();
                        field->setCountReference(reference);
                        field->resolveReferences(nullptr);
                    } else {
                        delete field;
                        field = nullptr;
                    }
                    if (field != nullptr) {
                        if (object.keys().contains("itemSize")) {
                            if (object.value("itemSize").isDouble()) {
                                int is = object.value("itemSize").toInt();
                                if (is < 0) {
                                    is = 0;
                                }
                                unsigned long itemSize = static_cast<unsigned long>(is);
                                field->setItemSize(itemSize);
                                field->setSize(field->getCount() * itemSize);
                            } else if (object.value("itemSize").isString()) {
                                QString reference = object.value("itemSize").toString();
                                field->setItemSizeReference(reference);
                            }
                        }
                        if (field->getSize() != 0) {
                            for (unsigned long i = 0; i < field->getSize(); i++) {
                                parseValue(object.value("item").toObject(), QString("item[%1]").arg(i),
                                                        field, static_cast<int>(i));
                            }
                        } else {
                            parseValue(object.value("item").toObject(), QString("item[0]"), field, 0);
                        }
                    }
                }
                break;
            case OBJECT_TYPE:
                if (object.keys().contains("fields") || !object.value("fields").isObject()) {
                    field = new JsonStoredData(data, offset, type, name, arrayIndex);
                    if (!resolved) {
                        field->setOffsetReference(offsetReference);
                    }
                    parseObject(object.value("fields").toObject(), field);
                }
                break;
            case IMAGE_TYPE: break;
            case IMAGE_SET_TYPE: break;
            default: break;
            }
        }
    }
    if (field != nullptr) {
        data->appendField(field);
    }
}

void StructureEditor::on_structureView_customContextMenuRequested(const QPoint &pos) {
    QMenu menu;
    QAction *saveToFileAction = new QAction("Save to file");
    menu.addAction(saveToFileAction);
    QAction *result = menu.exec(ui->structureView->mapToGlobal(pos));
    if (result == saveToFileAction) {
        void *pointer = ui->structureView->selectionModel()->currentIndex().internalPointer();
        if (pointer != nullptr) {
            JsonStoredData *data = static_cast<JsonStoredData *>(pointer);
            if (data != nullptr) {
                QString filename = QFileDialog::getSaveFileName(this, tr("Select files"), nullptr, "*.*", nullptr,
                                                                QFileDialog::DontUseNativeDialog);
                if (!filename.isEmpty()) {
                    auto bin = data->getValue().toByteArray();
                    QFile f(filename);
                    if (bin.size() > 0 && f.open(QFile::WriteOnly)) {
                        f.write(bin);
                        f.close();
                    } else {
                        StructureByteArray array;
                        JsonStoredDataHelper::objectToBinary(data, &array);
                        if (array.size() > 0 && f.open(QFile::WriteOnly)) {
                            f.write(array);
                            f.close();
                        }
                    }
                }
            }
        }
    }
    delete saveToFileAction;
}
