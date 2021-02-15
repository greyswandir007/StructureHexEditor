#ifndef STRUCTUREEDITOR_H
#define STRUCTUREEDITOR_H

#include <QWidget>

namespace Ui {
class StructureEditor;
}

class HexEditor;
struct JsonStoredData;

class StructureEditor : public QWidget
{
    Q_OBJECT

public:
    explicit StructureEditor(QWidget *parent = nullptr);
    ~StructureEditor();

    void parseJSONDocument(QString json);
    QString formatJSONDocument(QString json);
    void setHexEditor(HexEditor *hexEditor);

private:
    QJsonArray addExtensions(const QJsonArray &array);
    QJsonValue checkSignature(QString signature, unsigned long baseOffset = 0);
    QJsonObject parseKeys(const QJsonObject &jsonObject, unsigned long baseOffset = 0, const QStringList &keys = QStringList());
    QJsonValue parseObject(const QJsonObject &object, unsigned long baseOffset = 0);

    Ui::StructureEditor *ui;
    HexEditor *hexEditor;
    QTextCodec *utf8Codec;
    JsonStoredData *jsonStoredData = nullptr;
};

#endif // STRUCTUREEDITOR_H
