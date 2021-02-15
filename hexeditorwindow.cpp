#include "hexeditorwindow.h"
#include "ui_hexeditorwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextCodec>
#include <components/propertiesdialog.h>

HexEditorWindow::HexEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HexEditorWindow) {
    ui->setupUi(this);
    ui->mainSplitter->setStretchFactor(0, 4);
    ui->mainSplitter->setStretchFactor(1, 4);
    ui->mainSplitter->setStretchFactor(2, 1);
    ui->rightFilePanel->hide();
    ui->structureEditor->setHexEditor(ui->leftFilePanel);

    QScrollBar *scroll1 = ui->leftFilePanel->getScrollbar();
    QScrollBar *scroll2 = ui->rightFilePanel->getScrollbar();
    // Взаимно синхронизировать оба QTableView по прокрутке
    connect(scroll1, SIGNAL(valueChanged(int)), scroll2, SLOT(setValue(int)));
    connect(scroll2, SIGNAL(valueChanged(int)), scroll1, SLOT(setValue(int)));
    // И синхронизировать по позиционированию курсора в HEX-таблице
    connect(ui->leftFilePanel->selectionModel(), SIGNAL(currentChanged(const QModelIndex&,const QModelIndex&)),
            this, SLOT(syncEdit1to2(const QModelIndex&,const QModelIndex&)));

    connect(ui->rightFilePanel->selectionModel(), SIGNAL(currentChanged(const QModelIndex&,const QModelIndex&)),
            this, SLOT(syncEdit2to1(const QModelIndex&,const QModelIndex&)));

    QSettings settings("StructureHexEditor", "StructureHexEditor");
    settings.sync();
    int size = settings.beginReadArray("recentOpenedFiles");
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        QString filename = settings.value("filePath").toString();
        if (!filename.isEmpty()) {
            addRecentOpenedFile(filename);
        }
    }
    size = settings.beginReadArray("recentStructureFiles");
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        QString filename = settings.value("filePath").toString();
        if (!filename.isEmpty()) {
            addRecentStructureFile(filename);
        }
    }
}

HexEditorWindow::~HexEditorWindow() {
    QSettings settings("StructureHexEditor", "StructureHexEditor");
    settings.sync();
    settings.beginWriteArray("recentOpenedFiles", recentFilePaths.size());
    for (int i = 0; i < recentFilePaths.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("filePath", recentFilePaths.at(i));
    }
    settings.beginWriteArray("recentStructureFiles", recentStructurePaths.size());
    for (int i = 0; i < recentStructurePaths.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("filePath", recentStructurePaths.at(i));
    }
    settings.sync();
    delete ui;
}

void HexEditorWindow::on_actionOpen_triggered() {
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files"), lastDirectory, "*.*", nullptr,
                                                      QFileDialog::DontUseNativeDialog);
    switch(files.size()) {
        case 0: {
            qDebug() << "No files selected";
            return;
        }
        case 1: {
            addRecentOpenedFile(files.at(0));
            lastDirectory = QFileInfo(files.at(0)).absolutePath();
            openHexFile(files.at(0));
        } break;
        case 2: {
            addRecentOpenedFile(files.at(0));
            addRecentOpenedFile(files.at(1));
            lastDirectory = QFileInfo(files.at(0)).absolutePath();
            ui->leftFilePanel->loadFile(files.at(0));
            ui->rightFilePanel->loadFile(files.at(1));
            ui->rightFilePanel->show();

            if (ui->leftFilePanel->getBinaryData()->size() != ui->rightFilePanel->getBinaryData()->size()) {
                QMessageBox::warning(nullptr, tr("Warning"), tr("Are you aware that files DO differ in size?"));
            }
            ui->leftFilePanel->setCompareData(ui->rightFilePanel->getBinaryData());
            ui->rightFilePanel->setCompareData(ui->leftFilePanel->getBinaryData());

            ui->actionSave2->setEnabled(true);
            ui->actionSave_As2->setEnabled(true);
            ui->actionLeftToRight->setEnabled(true);
            ui->actionCopy_Right_To_Left->setEnabled(true);
            ui->actionPaste->setEnabled(false);
        } break;
        default: {
            QMessageBox::warning(nullptr, tr("Warning"), tr("Too many files selected"));
        }

    }
}

void HexEditorWindow::on_actionSettings_triggered() {
    PropertiesDialog propertiesDialog;
    int mode = ui->leftFilePanel->getResizeMode();
    QString policy;
    if (mode == HEX_RESIZE_MODE_DYNAMIC) {
        policy = "Dynamic";
    } else if (mode == HEX_RESIZE_MODE_POW_BY_TWO) {
        policy = "Pow by 2";
    } else if (mode == HEX_RESIZE_MODE_FIXED) {
        policy = "Fixed";
    } else {
        policy = "Dynamic";
    }
    propertiesDialog.setItem(policy, ui->leftFilePanel->getColumnCount());
    if (propertiesDialog.exec() == QDialog::Accepted) {
        policy = propertiesDialog.getPolicy();
        if (policy == "Dynamic") {
            mode = HEX_RESIZE_MODE_DYNAMIC;
        } else if (policy == "Pow by 2") {
            mode = HEX_RESIZE_MODE_POW_BY_TWO;
        } else if (policy == "Fixed") {
            mode = HEX_RESIZE_MODE_FIXED;
        }
        if (mode == HEX_RESIZE_MODE_FIXED) {
            ui->leftFilePanel->setColumnCount(propertiesDialog.getFixedCount());
            ui->rightFilePanel->setColumnCount(propertiesDialog.getFixedCount());
        }
        ui->leftFilePanel->setResizeMode(mode);
        ui->rightFilePanel->setResizeMode(mode);
    }
}

void HexEditorWindow::on_actionSave_triggered() {
    ui->leftFilePanel->saveFile();
}

void HexEditorWindow::on_actionSave_As_triggered() {
    QString filter = tr("HEX File") + " (*.hex)" + ";;" + tr("Binary File") + " (*.bin)" + ";;" + tr("Any File") + " (*.*)";
    QString filename = QFileDialog::getSaveFileName(this, tr("Save as"), ".", filter);
    if (!filename.isEmpty()) {
        ui->leftFilePanel->saveFileAs(filename);
    }
}

void HexEditorWindow::on_actionSave2_triggered() {
    ui->rightFilePanel->saveFile();
}

void HexEditorWindow::on_actionSave_As2_triggered() {
    QString filter = tr("HEX File") + " (*.hex)" + ";;" + tr("Binary File") + " (*.bin)" + ";;" + tr("Any File") + " (*.*)";
    QString filename = QFileDialog::getSaveFileName(this, tr("Save as"), ".", filter);
    if (!filename.isEmpty()) {
        ui->rightFilePanel->saveFileAs(filename);
    }
}

void HexEditorWindow::on_actionPaste_File_triggered() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Select file to be inserted"), lastDirectory, "*.*");
    if (filename.isEmpty()) {
        return;
    }
    long size = ui->leftFilePanel->getBinaryData()->size();
    int offset = 0;
    if (execOffsetDialog(tr("Input paste offset"), size, &offset) == QInputDialog::Accepted) {
        if (offset >= size) {
            ui->leftFilePanel->appendBinaryFile(filename, offset);
        } else {
            int res = QMessageBox::question(this, tr("Warning"), "Overwrite data?", QMessageBox::Yes | QMessageBox::No);
            if (res == QMessageBox::Yes) {
                ui->leftFilePanel->appendBinaryFile(filename, offset);
            }
        }
    }
}

void HexEditorWindow::on_actionLeftToRight_triggered() {
    copyData(ui->leftFilePanel, ui->rightFilePanel);
}

void HexEditorWindow::on_actionCopy_Right_To_Left_triggered() {
    copyData(ui->rightFilePanel, ui->leftFilePanel);
}

void HexEditorWindow::syncEdit1to2(const QModelIndex &current, const QModelIndex &prev) {
    if (current != prev) {
        ui->rightFilePanel->selectionModel()
                ->setCurrentIndex(current, QItemSelectionModel::Current | QItemSelectionModel::Select);
    }
}

void HexEditorWindow::syncEdit2to1(const QModelIndex &current, const QModelIndex &prev) {
    if (current != prev) {
        ui->leftFilePanel->selectionModel()
                ->setCurrentIndex(current, QItemSelectionModel::Current | QItemSelectionModel::Select);
    }
}

int HexEditorWindow::execOffsetDialog(QString header, int size, int *value) {
    QInputDialog inputDialog;
    inputDialog.setWindowTitle(header);
    inputDialog.setLabelText(tr("Starting with address") + " " + QString("%1").arg(size));
    inputDialog.setInputMode(inputDialog.IntInput);
    inputDialog.setIntMaximum(MAX_OFFSET);
    inputDialog.setIntMinimum(0);
    inputDialog.setIntValue(size);
    int result = inputDialog.exec();
    *value = inputDialog.intValue();
    return result;
}

void HexEditorWindow::copyData(HexEditor *from, HexEditor *to) {
    QModelIndexList indexes = from->selectionModel()->selectedIndexes();
    if (indexes.size() < 1) {
        QMessageBox::information(this, tr("Warning"), tr("No cells selected"));
    } else {
        int start = indexes.at(0).row() * from->getColumnCount() + indexes.at(0).column() - 1;
        int end = indexes.last().row() * from->getColumnCount() + indexes.last().column() - 1;
        long size = to->getBinaryData()->size();
        int offset = 0;
        if (execOffsetDialog(tr("Copy offset"), size, &offset) == QInputDialog::Accepted) {
            to->copy(offset, from->getBinaryData(), start, end - start + 1);
        }
    }
}

void HexEditorWindow::addRecentOpenedFile(QString filename) {
    if (recentFilePaths.contains(filename)) {
        return;
    }
    recentFilePaths.append(filename);
    if (ui->menuOpen_Recent->actions().size() == 1 &&
        ui->menuOpen_Recent->actions().at(0)->text() == "No recent items") {
        ui->menuOpen_Recent->actions()[0]->setText(filename);
        ui->menuOpen_Recent->actions()[0]->setEnabled(true);
        connect(ui->menuOpen_Recent->actions()[0], SIGNAL(triggered()), this,
                SLOT(openRecentTriggered()));
    } else {
        QAction *action = new QAction();
        action->setText(filename);
        connect(action, SIGNAL(triggered()), this, SLOT(openRecentTriggered()));
        ui->menuOpen_Recent->addAction(action);
    }
}

void HexEditorWindow::addRecentStructureFile(QString filename) {
    if (recentStructurePaths.contains(filename)) {
        return;
    }
    recentStructurePaths.append(filename);
    if (ui->menuOpen_Recent_2->actions().size() == 1 &&
        ui->menuOpen_Recent_2->actions().at(0)->text() == "No recent items") {
        ui->menuOpen_Recent_2->actions()[0]->setText(filename);
        ui->menuOpen_Recent_2->actions()[0]->setEnabled(true);
        connect(ui->menuOpen_Recent_2->actions()[0], SIGNAL(triggered()), this,
                SLOT(openRecent2Triggered()));
    } else {
        QAction *action = new QAction();
        action->setText(filename);
        connect(action, SIGNAL(triggered()), this, SLOT(openRecent2Triggered()));
        ui->menuOpen_Recent_2->addAction(action);
    }
}

void HexEditorWindow::openHexFile(QString filename) {
    ui->leftFilePanel->loadFile(filename);
    ui->rightFilePanel->hide();
    ui->actionSave2->setEnabled(false);
    ui->actionSave_As2->setEnabled(false);
    ui->actionLeftToRight->setEnabled(false);
    ui->actionCopy_Right_To_Left->setEnabled(false);
    ui->leftFilePanel->setCompareData(nullptr);
    ui->rightFilePanel->setCompareData(nullptr);
    ui->actionPaste->setEnabled(true);
}

void HexEditorWindow::openStructureFile(QString filename) {
    QFile f(filename);
    if (f.open(QFile::ReadOnly)) {
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        ui->structureEdit->setText(ui->structureEditor->formatJSONDocument(codec->toUnicode(f.readAll())));
        f.close();
        structureFilename = filename;
        ui->structureFilename->setText(structureFilename);
    }
}

void HexEditorWindow::on_newStructureButton_clicked() {
    structureFilename = "";
    ui->structureFilename->setText("");
    ui->structureEdit->clear();
}

void HexEditorWindow::on_actionNew_2_triggered() {
    on_newStructureButton_clicked();
}

void HexEditorWindow::on_openStructureButton_clicked() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Select signature file"), lastDirectory,
                                                    "JSON File (*.json)", nullptr, QFileDialog::DontUseNativeDialog);
    if (!filename.isEmpty()) {
        addRecentStructureFile(filename);
        openStructureFile(filename);
    }
}

void HexEditorWindow::on_actionOpen_2_triggered() {
    on_openStructureButton_clicked();
}

void HexEditorWindow::on_saveStructureButton_clicked() {
    if (structureFilename.isEmpty()) {
        QString filename = QFileDialog::getSaveFileName(this, tr("Select signature file"), lastDirectory,
                                                        "JSON File (*.json)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isEmpty()) {
            if (!QFileInfo(filename).fileName().contains(".")) {
                filename.append(".json");
            }
            structureFilename = filename;
            ui->structureFilename->setText(structureFilename);
            QFile f(filename);
            if (f.open(QFile::WriteOnly)) {
                QTextCodec *codec = QTextCodec::codecForName("UTF-8");
                f.write(codec->fromUnicode(ui->structureEdit->toPlainText()));
                f.close();
            }
        }
    } else {
        QFile f(structureFilename);
        if (f.open(QFile::WriteOnly)) {
            QTextCodec *codec = QTextCodec::codecForName("UTF-8");
            f.write(codec->fromUnicode(ui->structureEdit->toPlainText()));
            f.close();
        }
    }
}

void HexEditorWindow::on_actionSave_2_triggered() {
    on_saveStructureButton_clicked();
}

void HexEditorWindow::on_saveAsStructreButton_clicked() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Select signature file"), lastDirectory,
                                                    "JSON File (*.json)", nullptr, QFileDialog::DontUseNativeDialog);
    if (!filename.isEmpty()) {
        if (!QFileInfo(filename).fileName().contains(".")) {
            filename.append(".json");
        }
        structureFilename = filename;
        ui->structureFilename->setText(structureFilename);
        QFile f(filename);
        if (f.open(QFile::WriteOnly)) {
            QTextCodec *codec = QTextCodec::codecForName("UTF-8");
            f.write(codec->fromUnicode(ui->structureEdit->toPlainText()));
            f.close();
        }
    }
}

void HexEditorWindow::on_actionSave_As_2_triggered() {
    on_saveAsStructreButton_clicked();
}

void HexEditorWindow::openRecentTriggered() {
    if (QAction * action = qobject_cast<QAction *>(QObject::sender())) {
        openHexFile(action->text());
    }
}

void HexEditorWindow::openRecent2Triggered() {
    if (QAction * action = qobject_cast<QAction *>(QObject::sender())) {
        openStructureFile(action->text());
    }
}

void HexEditorWindow::on_applyStructureButton_clicked() {
    ui->structureEditor->parseJSONDocument(ui->structureEdit->toPlainText());
}

void HexEditorWindow::on_actionExit_triggered() {
    QApplication::quit();
}

void HexEditorWindow::on_formatButton_clicked() {
    ui->structureEdit->setText(ui->structureEditor->formatJSONDocument(ui->structureEdit->toPlainText()));
}
