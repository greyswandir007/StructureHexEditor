#ifndef PALETTEDIALOG_H
#define PALETTEDIALOG_H

#include <QDialog>

namespace Ui {
class PaletteDialog;
}

class StructureEditor;
struct StructureNamedItem;

class PaletteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PaletteDialog(unsigned int *pal = nullptr, QWidget *parent = nullptr);
    ~PaletteDialog();

    static unsigned int *getDefaultPalette();

    unsigned int *getPaletteCopy();
    void setStructureEditor(StructureEditor *editor);
    void setStructureNamedItems(const QList<StructureNamedItem> &value);

private slots:
    void colorButtonClicked();
    void on_loadButton_clicked();
    void on_saveButton_clicked();
    void on_resetButton_clicked();
    void on_readButton_clicked();

private:
    QPixmap emptyPixmap(int size, QColor color);

    Ui::PaletteDialog *ui;
    QList<QPushButton*> buttonList;
    unsigned int palette[256];
    StructureEditor *structureEditor = nullptr;
    QList<StructureNamedItem> structureNamedItems;
};

#endif // PALETTEDIALOG_H
