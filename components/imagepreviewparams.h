#ifndef IMAGEPREVIEWPARAMS_H
#define IMAGEPREVIEWPARAMS_H

struct ImagePreviewParams {
    int width;
    int height;
    int itemCount;
    int itemsInRow;
    unsigned int skipBytesCount;
    unsigned int gapBytesCount;
    int depthIndex;
};

#endif // IMAGEPREVIEWPARAMS_H
