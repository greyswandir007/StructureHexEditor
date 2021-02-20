#ifndef GOTODIALOG_H
#define GOTODIALOG_H

#include <QDialog>

namespace Ui {
class GoToDialog;
}

class GoToDialog : public QDialog {
    Q_OBJECT

public:
    explicit GoToDialog(QWidget *parent = nullptr);
    ~GoToDialog();

    unsigned int getValue();

private:
    Ui::GoToDialog *ui;
};

#endif // GOTODIALOG_H
