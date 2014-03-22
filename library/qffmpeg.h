#ifndef QFFMPEG_H
#define QFFMPEG_H

#include "library_global.h"

#include <iostream>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QImage>
#include <QBuffer>
#include <QDebug>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
};

#include "qavframe.h"
#include "../Analyzer/analyzer.h"

static bool initCodec = false;

class LIBRARYSHARED_EXPORT QFfmpeg : public QObject
{
    Q_OBJECT

public:
    explicit QFfmpeg(const QString &filename, QObject *parent = 0);
    virtual ~QFfmpeg();

    bool isOk(void) { return initCodec; }
    bool decodeAudio();
    bool close();
    void dumpFormat();

    QByteArray getPicture();

    // Global Functions
    bool parse_metaData();
    QString metaData(const QString &tagName);
    QString getFormat();
    QString getStringDuration();
    int getDuration();
    int getBitrate();

    // Audio Functions
    AVStream *getAudioStream() { return getStream(AVMEDIA_TYPE_AUDIO); }
    int getAudioChannelCount();
    int getAudioSamplerate();
    int getAudioBitrate();
    QStringList getAudioLanguages() { return getStreamLanguages(AVMEDIA_TYPE_AUDIO); }
    QString getAudioFormat();

    // Video Functions
    AVStream *getVideoStream() { return getStream(AVMEDIA_TYPE_VIDEO); }
    QString getVideoResolution();
    double getVideoFrameRate();
    QStringList getVideoLanguages() { return getStreamLanguages(AVMEDIA_TYPE_VIDEO); }

    // Subtitle Functions
    QStringList getSubtitleLanguages() { return getStreamLanguages(AVMEDIA_TYPE_SUBTITLE); }


private:
    QString filename;

    AVFormatContext *formatContext;
    int size;

    QHash<QString, QString> *global_metaData;
    int64_t duration;
    int bitrate;
    QString containerFormat;
    QString audioFormat;
    int audioChannelCount;
    int audioSampleRate;
    int audioBitRate;
    QByteArray picture;
    QHash<AVMediaType, QStringList> *streamLanguages;

    // private functions
    bool open();
    bool isOpened() { return formatContext != NULL; }


    AVStream *getStream(const AVMediaType &mediaType);
    QStringList getStreamLanguages(const AVMediaType &mediaType);
};

#endif // QFFMPEG_H
