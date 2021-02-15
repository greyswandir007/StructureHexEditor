#ifndef HEXEDITORWINDOW_H
#define HEXEDITORWINDOW_H

#include <QMainWindow>

namespace Ui {
class HexEditorWindow;
}

class HexEditor;

class HexEditorWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit HexEditorWindow(QWidget *parent = nullptr);
    ~HexEditorWindow();

private slots:
    void on_actionOpen_triggered();
    void on_actionSettings_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionSave2_triggered();
    void on_actionSave_As2_triggered();
    void on_actionPaste_File_triggered();
    void on_actionLeftToRight_triggered();
    void on_actionCopy_Right_To_Left_triggered();
    void syncEdit1to2(const QModelIndex &current, const QModelIndex &prev);
    void syncEdit2to1(const QModelIndex &current, const QModelIndex &prev);
    void on_newStructureButton_clicked();
    void on_actionNew_2_triggered();
    void on_openStructureButton_clicked();
    void on_actionOpen_2_triggered();
    void on_saveStructureButton_clicked();
    void on_actionSave_2_triggered();
    void on_saveAsStructreButton_clicked();
    void on_actionSave_As_2_triggered();
    void openRecentTriggered();
    void openRecent2Triggered();
    void on_applyStructureButton_clicked();

    void on_actionExit_triggered();

    void on_formatButton_clicked();

private:
    static const long MAX_OFFSET = 10000000;

    int execOffsetDialog(QString header, int size, int *value);
    void copyData(HexEditor *from, HexEditor *to);
    void addRecentOpenedFile(QString filename);
    void addRecentStructureFile(QString filename);
    void openHexFile(QString filename);
    void openStructureFile(QString filename);

    Ui::HexEditorWindow *ui;
    QString lastDirectory;
    QString structureFilename = "";
    QStringList recentFilePaths;
    QStringList recentStructurePaths;
};

#endif // HEXEDITORWINDOW_H