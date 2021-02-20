#ifndef HEXEDITORWINDOW_H
#define HEXEDITORWINDOW_H

#include <QMainWindow>

#include "components/jsonhighlighter.h"
#include "components/structurenameditem.h"

namespace Ui {
class HexEditorWindow;
}

class HexEditor;
class StructureByteArray;

class HexEditorWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit HexEditorWindow(QWidget *parent = nullptr);
    ~HexEditorWindow();
    void resizeEvent(QResizeEvent* event);

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
    void on_saveAsStructureButton_clicked();
    void on_actionSave_As_2_triggered();
    void openRecentTriggered();
    void openRecent2Triggered();
    void on_applyStructureButton_clicked();
    void on_actionExit_triggered();
    void on_formatStructureButton_clicked();
    void on_previewButton_clicked();
    void on_imagePaletteButton_clicked();
    void on_imageSaveButton_clicked();
    void on_imageSaveRawButton_clicked();
    void on_actionApply_Structure_triggered();
    void on_actionFormat_Structure_triggered();

    void on_hexPreviewButton_clicked();

    void on_hexSourceBox_activated(int index);

private:
    static const int MAX_OFFSET = 10000000;

    void loadSetting();
    void saveSettings();
    void addSource(QString fullName, QString displayName);
    void clearSources();

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
    unsigned int *currentPalette;

    QList<StructureNamedItem> structureNamedItems;

    JsonHighlighter *highlighter = nullptr;
};

#endif // HEXEDITORWINDOW_H
