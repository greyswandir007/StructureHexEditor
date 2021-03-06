#ifndef STRUCTUREEDITOR_H
#define STRUCTUREEDITOR_H

#include <QWidget>

#include <components/jsontreemodel.h>

namespace Ui {
class StructureEditor;
}

class HexEditor;

class StructureEditor : public QWidget
{
    Q_OBJECT

public:
    explicit StructureEditor(QWidget *parent = nullptr);
    ~StructureEditor();

    void parseJSONDocument(QString json);
    QString formatJSONDocument(QString json);
    void setHexEditor(HexEditor *hexEditor);
    void clearStructure();

    QList<JsonStoredData *> getBinaryList();
    JsonStoredData *getStoredDataByName(QString name);

    void saveStoredData(JsonStoredData *data);

private slots:
    void on_structureView_customContextMenuRequested(const QPoint &pos);

private:
    QStringList addExtensions(const QJsonArray &array);
    void parseObject(const QJsonObject &object, JsonStoredData *data, const QStringList &keys = QStringList());
    void parseValue(const QJsonObject &object, QString name, JsonStoredData *data, int arrayIndex = -1);

    Ui::StructureEditor *ui;
    HexEditor *hexEditor;
    QTextCodec *utf8Codec;
    JsonStoredData *jsonStoredData = nullptr;
    JsonTreeModel *model;
};

#endif // STRUCTUREEDITOR_H
