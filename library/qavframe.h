#ifndef QAVFRAME_H
#define QAVFRAME_H

#include <QObject>
#include <QImage>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/frame.h"
#include "libswscale/swscale.h"
}

#include <iostream>

class QAVFrame : public QObject
{
    Q_OBJECT

public:
    explicit QAVFrame(QObject *parent = 0);
    virtual ~QAVFrame();

    bool isNull() { return frame == NULL; }
    AVFrame *ptr() { return frame; }

    QImage toImage(int width, int height);

signals:

public slots:

private:
    AVFrame *frame;
};

#endif // QAVFRAME_H
