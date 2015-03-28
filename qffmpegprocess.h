#ifndef QFFMPEGPROCESS_H
#define QFFMPEGPROCESS_H

#include <QObject>
#include <QProcess>
#include <QtXml>

#include "../Analyzer/analyzer.h"

class QFfmpegProcess : public QObject
{
    Q_OBJECT

public:
    explicit QFfmpegProcess(const QString &filename, QObject *parent = 0);
    ~QFfmpegProcess();

    QString metaData(const QString &tagName) const;
    QString getFormat() const;
    int getBitrate() const;
    qint64 getDuration() const;
    QByteArray getPicture() const;

    int getAudioChannelCount() const;
    int getAudioSamplerate() const;
    int getAudioBitrate() const;
    QString getAudioFormat() const;

    QString getVideoResolution() const;
    double getVideoFrameRate() const;

    QStringList getAudioLanguages() const { return getSreamsTag("audio", "language"); }
    QStringList getVideoLanguages() const { return getSreamsTag("video", "language"); }
    QStringList getSubtitleLanguages() const { return getSreamsTag("subtitle", "language"); }

private:
    QByteArray parsePicture() const;
    QStringList getSreamsTag(const QString &codec_type, const QString &tagName) const;

signals:

public slots:
    void probeFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QString filename;
    QProcess programFfmpegProbe;
    QDomDocument xmlResProbe;
    QDomNode audioStream;
    QDomNode videoStream;
    QByteArray picture;
};

#endif // QFFMPEGPROCESS_H
