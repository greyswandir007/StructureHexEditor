#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

namespace Ui {
class FindDialog;
}

class StructureByteArray;

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog();

    StructureByteArray getSearchValue();
    bool continueSearch();

private slots:
    void on_findTextEdit_textChanged(const QString &arg1);
    void on_findHexEdit_textChanged(const QString &arg1);

private:
    Ui::FindDialog *ui;
    bool textChanged = false;
};

#endif // FINDDIALOG_H
