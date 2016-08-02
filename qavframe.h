#ifndef QAVFRAME_H
#define QAVFRAME_H

#include <QObject>
#include <QImage>
#include <QBuffer>

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

    bool isNull() const { return frame == NULL; }
    AVFrame *ptr() const { return frame; }

    QImage toImage(const int &width, const int &height) const;
    QByteArray toByteArray(const int &width, const int &height) const;

signals:

public slots:

private:
    AVFrame *frame;
};

#endif // QAVFRAME_H
