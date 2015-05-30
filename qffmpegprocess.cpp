#include "qffmpegprocess.h"

QFfmpegProcess::QFfmpegProcess(const QString &filename, QObject *parent):
    QObject(parent),
    filename(filename),
    programFfmpegProbe(this),
    xmlResProbe(),
    audioStream(),
    videoStream(),
    picture()
{
    ANALYZER

    connect(&programFfmpegProbe, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(probeFinished(int,QProcess::ExitStatus)));

    programFfmpegProbe.setProgram("/opt/local/bin/ffprobe");
    QStringList argFfmpeg;
    argFfmpeg << "-i" << filename;
    argFfmpeg << "-show_format";
    argFfmpeg << "-show_entries" << "stream";
    argFfmpeg << "-of" << "xml";
    programFfmpegProbe.setArguments(argFfmpeg);
    programFfmpegProbe.start();
    if (!programFfmpegProbe.waitForStarted())
        qWarning() << "probe not started" << filename;

    picture = parsePicture();

    programFfmpegProbe.waitForFinished();

    ANALYZER_RETURN
}

QFfmpegProcess::~QFfmpegProcess()
{
    ANALYZER

    if (programFfmpegProbe.state() == QProcess::Running)
    {
        if (!programFfmpegProbe.waitForFinished())
            qWarning() << "unable to finish probe" << filename;
    }

    ANALYZER_RETURN
}

void QFfmpegProcess::probeFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ANALYZER

    Q_UNUSED(exitCode)

    xmlResProbe.clear();
    audioStream.clear();

    if (exitStatus == QProcess::NormalExit)
    {
        xmlResProbe.setContent(programFfmpegProbe.readAll());
//        qWarning() << xmlResProbe.toString();

        QDomNodeList streamList = xmlResProbe.elementsByTagName("stream");
        for (int index=0;index<streamList.count();++index)
        {
            QDomNode stream = streamList.at(index);
            if (stream.attributes().namedItem("codec_type").nodeValue() == "audio")
                audioStream = stream;
            else if (stream.attributes().namedItem("codec_type").nodeValue() == "video")
                videoStream = stream;
        }
    }
    else
    {
        qWarning() << "probe crashed" << filename;
    }

    ANALYZER_RETURN
}

QString QFfmpegProcess::metaData(const QString &tagName) const
{
    ANALYZER

    QDomNodeList tagList = xmlResProbe.elementsByTagName("format").at(0).childNodes();

    for (int index=0;index<tagList.count();++index)
    {
        QDomNode tag = tagList.at(index);
        if (tag.nodeName() == "tag" && tag.attributes().namedItem("key").nodeValue() == tagName)
        {
            ANALYZER_RETURN
            return tag.attributes().namedItem("value").nodeValue();
        }
    }

    ANALYZER_RETURN
    return QString();
}

QString QFfmpegProcess::getFormat() const
{
    QDomNode format = xmlResProbe.elementsByTagName("format").at(0);
    return format.attributes().namedItem("format_name").nodeValue();
}

int QFfmpegProcess::getBitrate() const
{
    QDomNode format = xmlResProbe.elementsByTagName("format").at(0);
    return format.attributes().namedItem("bit_rate").nodeValue().toInt();
}

QString QFfmpegProcess::getAudioFormat() const
{
    ANALYZER

    QString res = audioStream.attributes().namedItem("codec_name").nodeValue();

    ANALYZER_RETURN
    return res;
}

int QFfmpegProcess::getAudioBitrate() const
{
    ANALYZER

    int res = audioStream.attributes().namedItem("bit_rate").nodeValue().toInt();

    ANALYZER_RETURN
    return res;
}

int QFfmpegProcess::getAudioChannelCount() const
{
    ANALYZER

    int res = audioStream.attributes().namedItem("channels").nodeValue().toInt();

    ANALYZER_RETURN
    return res;
}

int QFfmpegProcess::getAudioSamplerate() const
{
    ANALYZER

    int res = audioStream.attributes().namedItem("sample_rate").nodeValue().toInt();

    ANALYZER_RETURN
    return res;
}

QString QFfmpegProcess::getVideoResolution() const
{
    QString width = videoStream.attributes().namedItem("width").nodeValue();
    QString height = videoStream.attributes().namedItem("height").nodeValue();
    if (!width.isEmpty() && !height.isEmpty())
        return QString("%1x%2").arg(width).arg(height);
    return QString();
}


double QFfmpegProcess::getVideoFrameRate() const
{
    QRegExp pattern("(\\d+)/(\\d+)");
    QString strFrameRate = videoStream.attributes().namedItem("avg_frame_rate").nodeValue();
    if (pattern.indexIn(strFrameRate) != -1 && pattern.cap(2).toDouble() != 0.0)
        return pattern.cap(1).toDouble()/pattern.cap(2).toDouble();
    else
        return 0.0;
}

QStringList QFfmpegProcess::getSreamsTag(const QString &codec_type, const QString &tagName) const
{
    QStringList res;

    QDomNodeList streamList = xmlResProbe.elementsByTagName("stream");
    for (int index=0;index<streamList.count();++index)
    {
        QDomNode stream = streamList.at(index);

        if (stream.attributes().namedItem("codec_type").nodeValue() == codec_type)
        {
            QString value;

            QDomNodeList tagList = stream.childNodes();
            for (int num_child=0;num_child<tagList.count();++num_child)
            {
                QDomNode tag = tagList.at(num_child);
                if (tag.nodeName() == "tag" && tag.attributes().namedItem("key").nodeValue().toLower() == tagName)
                    value = tag.attributes().namedItem("value").nodeValue().toLower();
            }

            res << value;
        }
    }

    return res;
}

qint64 QFfmpegProcess::getDuration() const
{
    ANALYZER

    qint64 res = xmlResProbe.elementsByTagName("format").at(0).attributes().namedItem("duration").nodeValue().toDouble()*1000.0;

    ANALYZER_RETURN
    return res;
}

QByteArray QFfmpegProcess::parsePicture() const
{
    ANALYZER

    QByteArray picture("");

    QProcess process;
    QString program = "/opt/local/bin/ffmpeg";
    QStringList arguments = QStringList() << "-i" << filename << "-f" << "mjpeg" << "-vframes" << "1" << "-s" << "300x300" << "-loglevel" << "error" << "pipe:";
    process.setProgram(program);
    process.setArguments(arguments);
    process.start();
    if (process.waitForFinished()) {
        if (process.exitCode() != 0) {
            QString error(process.readAllStandardError().trimmed());
//            if (error != "Output file #0 does not contain any stream")
//                qWarning() << "ERROR" << process.exitCode() << filename << error;
        } else {
            picture = process.readAllStandardOutput();

            if (picture.isEmpty())
                qWarning() << "NO PICTURE" << filename;

            ANALYZER_RETURN
            return picture;
        }
    }
    else
    {
        qWarning() << "NO PICTURE" << filename;
    }

    ANALYZER_RETURN
    return picture;
}

QByteArray QFfmpegProcess::getPicture() const {
    ANALYZER

    if (!picture.isNull()) {
        if (picture.isEmpty()) {
            ANALYZER_RETURN
            return QByteArray();
        } else {
            ANALYZER_RETURN
            return picture;
        }
    }

    ANALYZER_RETURN
    return picture;
}
