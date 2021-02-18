#include "mainview.h"
#include "structurebytearray.h"

#include <QFileDialog>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>

MainView::MainView(QWidget *parent) : QGraphicsView(parent) {
    setMouseTracking(true);
    setScene(new QGraphicsScene());
    updateSceneRect();
}

void MainView::updateSceneRect() {
    QRect rec(geometry());
    scene()->setSceneRect(0, 0, rec.width() - 3, rec.height() - 3);
}

void MainView::previewImage(ImagePreviewParams params, StructureByteArray *source) {
    switch (params.depthIndex) {
    case 0: break;
    case 1:
        scene()->clear();
        scene()->addPixmap(create8bitPixmap(params, source))->setScale(mainScale);
        break;
    case 2: break;
    case 3: break;
    case 4: break;
    case 5: break;
    default: break;
    }
}

QPixmap MainView::create8bitPixmap(ImagePreviewParams params, StructureByteArray *source) {
    int fullWitdh = params.width * params.itemsInRow;
    int fullHeight = params.height * (params.itemCount / params.itemsInRow
                                      + (params.itemCount % params.itemsInRow == 0 ? 0 : 1));
    QPixmap pix = QPixmap(fullWitdh, fullHeight);
    QPainter painter(&pix);
    painter.setBrush(QBrush(Qt::black));
    painter.setPen(QPen(Qt::black));
    painter.drawRect(0, 0, fullWitdh, fullHeight);
    unsigned int offset = params.skipBytesCount;
    int count = 0;
    int y = 0;
    while (count < params.itemCount) {
        int x = 0;
        for (int k = 0; k < params.itemsInRow && count < params.itemCount; k++) {
            for (int j = 0; j < params.height; j++) {
                for (int i = 0; i < params.width; i++) {
                    unsigned int color = currentPalette[source->ucharAt(offset++)];
                    painter.setPen(QPen(QColor(color)));
                    painter.drawPoint(x + i, y + j);
                }
            }
            x += params.width;
            count++;
            offset += params.gapBytesCount;
        }
        y += params.height;
    }
    painter.end();
    return pix;
}

unsigned int *MainView::getCurrentPalette() const {
    return currentPalette;
}

void MainView::setCurrentPalette(unsigned int *value) {
    currentPalette = value;
}

void MainView::saveImage() {
    if (scene()->items().size() > 0) {
        QString filename = QFileDialog::getSaveFileName(this, tr("Image file"), nullptr, "*.png", nullptr,
                                                          QFileDialog::DontUseNativeDialog);
        if (!filename.isEmpty()) {
            if (!filename.endsWith(".png")) {
                filename = filename + ".png";
            }
            QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(scene()->items().at(0));
            if (item != nullptr) {
                item->pixmap().save(filename);
            }
        }
    }
}

void MainView::mousePressEvent(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);
    if (event->button() == Qt::MidButton) {
        mainScale = 1;
        changeScale(mainScale);
        event->accept();
    }
}

void MainView::wheelEvent(QWheelEvent *event) {
    QGraphicsView::wheelEvent(event);
    double f = event->delta() / 120;
    double power = pow(1.1, fabs(f));
    mainScale = f > 0 ? mainScale * power : mainScale / power;
    changeScale(mainScale);
    event->accept();
}

void MainView::changeScale(double scale) {
    for(QGraphicsItem *item : scene()->items()) {
        item->setScale(scale);
    }
}


