#ifndef QFFMPEG_H
#define QFFMPEG_H

#include <iostream>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QImage>
#include <QBuffer>
#include <QDebug>
#include <QProcess>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <libavutil/timestamp.h>
};

#include "qavframe.h"
#include "../Analyzer/analyzer.h"

static bool initCodec = false;

class QFfmpeg : public QObject
{
    Q_OBJECT

public:
    explicit QFfmpeg(const QString &filename, QObject *parent = 0);
    virtual ~QFfmpeg();

    bool isNull(void) const { return !initCodec; }
    bool close();
    void dumpFormat();

    // Global Functions
    QString metaData(const QString &tagName) const;
    QString getFormat() const;
    QString getStringDuration() const;
    int64_t getDuration() const;
    int getBitrate() const;

    int decodeStream(uint stream_idx);  // decode stream and return the number of frames decoded
    AVStream *getStream(const AVMediaType &mediaType) const;
    QStringList getStreamLanguages(const AVMediaType &mediaType) const;

    // Audio Functions
    AVStream *getAudioStream() const { return getStream(AVMEDIA_TYPE_AUDIO); }
    int getAudioChannelCount() const;
    int getAudioSamplerate() const;
    int getAudioBitrate() const;
    QStringList getAudioLanguages() const { return getStreamLanguages(AVMEDIA_TYPE_AUDIO); }
    QString getAudioFormat() const;

    // Video Functions
    AVStream *getVideoStream() const { return getStream(AVMEDIA_TYPE_VIDEO); }
    QString getVideoResolution() const;
    double getVideoFrameRate() const;
    QStringList getVideoLanguages() const { return getStreamLanguages(AVMEDIA_TYPE_VIDEO); }
    QByteArray getPicture() const;

    // Subtitle Functions
    QStringList getSubtitleLanguages() const { return getStreamLanguages(AVMEDIA_TYPE_SUBTITLE); }


private:
    QString filename;

    AVFormatContext *formatContext;
    QAVFrame frame;
    int size;

    QString audioFormat;
    int audioChannelCount;
    int audioSampleRate;
    int audioBitRate;
    QByteArray picture;
    QHash<AVMediaType, QStringList> *streamLanguages;

    // private functions
    bool open();
    bool isOpened() const { return formatContext != NULL; }

    int resultStreamInfoParsed;
    int findStreamInfo();

    int openCodecContext(const AVMediaType &mediaType);
    int openCodecContext(const uint &stream_idx);

    bool decodePacket(AVPacket *pkt);

    QByteArray parsePictureByProcess() const;
    QByteArray parsePictureByApi();


};

#endif // QFFMPEG_H
