#ifndef PALETTEDIALOG_H
#define PALETTEDIALOG_H

#include <QDialog>

namespace Ui {
class PaletteDialog;
}

class PaletteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PaletteDialog(unsigned int *pal = nullptr, QWidget *parent = nullptr);
    ~PaletteDialog();

    static unsigned int *getDefaultPalette();

    unsigned int *getPaletteCopy();

private slots:
    void colorButtonClicked();
    void on_loadButton_clicked();
    void on_saveButton_clicked();
    void on_resetButton_clicked();

private:
    QPixmap emptyPixmap(int size, QColor color);

    Ui::PaletteDialog *ui;
    QList<QPushButton*> buttonList;
    unsigned int palette[256];
};

#endif // PALETTEDIALOG_H
