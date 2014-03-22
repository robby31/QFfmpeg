#include "qavframe.h"

QAVFrame::QAVFrame(QObject *parent) :
    QObject(parent),
    frame(NULL)
{
    frame = avcodec_alloc_frame();
}

QAVFrame::~QAVFrame() {
    av_free(frame);
}

QImage QAVFrame::toImage(int width, int height) {
    if (frame == NULL) {
        return QImage();
    }

    // Allocate an AVFrame structure
    QAVFrame pFrameRGB;
    if (pFrameRGB.isNull()) {
        return QImage();
    }

    PixelFormat imageFormat = PIX_FMT_RGB24;

    uint8_t *buffer;
    int numBytes;
    // Determine required buffer size and allocate buffer
    numBytes = avpicture_get_size(PIX_FMT_RGB24, width, height);
    buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)pFrameRGB.ptr(), buffer,
                   PIX_FMT_RGB24, width, height);

    QImage img(width, height, QImage::Format_RGB32);

    // TODO: cache following conversion context for speedup,
    //       and recalculate only on dimension changes
    SwsContext *img_convert_ctx_temp = NULL;
    img_convert_ctx_temp = sws_getContext(frame->width, frame->height, (PixelFormat) frame->format,
                                          width, height, imageFormat,
                                          SWS_BICUBIC, NULL, NULL, NULL);

    sws_scale(img_convert_ctx_temp,
              frame->data, frame->linesize, 0, frame->height,
              pFrameRGB.ptr()->data, pFrameRGB.ptr()->linesize);

    uint8_t *src = (uint8_t *) pFrameRGB.ptr()->data[0];
    for (int y = 0; y < height; y++)
    {
        QRgb *scanLine = (QRgb *) img.scanLine(y);
        for (int x = 0; x < width; x=x+1)
        {
            scanLine[x] = qRgb(src[3*x], src[3*x+1], src[3*x+2]);
        }
        src += pFrameRGB.ptr()->linesize[0];
    }

    sws_freeContext(img_convert_ctx_temp);
    av_free(buffer);

    return img;
}
