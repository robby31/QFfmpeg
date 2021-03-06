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
    explicit QFfmpegProcess(QObject *parent = 0);
    explicit QFfmpegProcess(const QString &filename, QObject *parent = 0);
    virtual ~QFfmpegProcess() Q_DECL_OVERRIDE;

    bool isValid() const;

    QString metaData(const QString &tagName) const;
    QString getFormat() const;
    int getBitrate() const;
    qint64 getDuration() const;
    QByteArray getPicture() const;

    int getAudioChannelCount() const;
    int getAudioSamplerate() const;
    int getAudioBitrate() const;
    QString getAudioFormat() const;
    QHash<QString, double> getVolumeInfo(const int timeout = 30000);

    QString getVideoResolution() const;
    double getVideoFrameRate() const;

    QStringList getAudioLanguages() const { return getStreamsTag("audio", "language"); }
    QStringList getVideoLanguages() const { return getStreamsTag("video", "language"); }
    QStringList getSubtitleLanguages() const { return getStreamsTag("subtitle", "language"); }

    qint64 size() const;

    static void setDirPath(const QString &folder);

    bool setFilename(const QString &filename, const bool &readPicture = false);

private:
    bool waitProbeFinished() const;
    void parsePicture();
    bool waitPictureFinished() const;
    QStringList getStreamsTag(const QString &codec_type, const QString &tagName) const;

signals:
    void mediaReady();    // signal emitted when probe on media is done and picture parsed

public slots:
    void probeFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void pictureFinished(int exitCode, QProcess::ExitStatus exitStatus);

private slots:
    void probeDestroyed();
    void pictureDestroyed();

private:
    static QString EXE_DIRPATH;
    QProcess *programFfmpegProbe;
    QProcess *programFfmpegPicture;
    QString filename;
    QDomDocument xmlResProbe;
    QDomNode audioStream;
    QDomNode videoStream;
    QByteArray *picture;
};

#endif // QFFMPEGPROCESS_H
