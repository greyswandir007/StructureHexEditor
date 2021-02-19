#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "imagepreviewparams.h"

#include <QGraphicsView>
#include <QObject>

class StructureByteArray;

class MainView : public QGraphicsView {
    Q_OBJECT
public:
    MainView(QWidget *parent);

    void updateSceneRect();

    void previewImage(ImagePreviewParams params, StructureByteArray *source);

    QPixmap create8bitPixmap(ImagePreviewParams params, StructureByteArray *source);

    unsigned int *getCurrentPalette() const;
    void setCurrentPalette(unsigned int *value);
    void saveImage();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

private:
    void changeScale(double scale);

    unsigned int *currentPalette;
    double mainScale = 1;
};

#endif // MAINVIEW_H
