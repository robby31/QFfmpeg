#include "qffmpegprocess.h"

QString QFfmpegProcess::EXE_DIRPATH = "";

void QFfmpegProcess::setDirPath(const QString &folder)
{
    EXE_DIRPATH = folder;
}

QFfmpegProcess::QFfmpegProcess(QObject *parent):
    QObject(parent),
    programFfmpegProbe(this),
    filename(),
    xmlResProbe(),
    audioStream(),
    videoStream(),
    picture()
{
    ANALYZER

    if (EXE_DIRPATH.isEmpty())
    {
        qCritical() << "QFFmpeg, invalid installation path where are located FFMPEG binaries.";
    }
    else
    {
        QDir folder(EXE_DIRPATH);
        if (!folder.exists())
        {
            qCritical() << "QFFmpeg, invalid installation path where are located FFMPEG binaries:" << folder;
        }
        else
        {
            connect(&programFfmpegProbe, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(probeFinished(int,QProcess::ExitStatus)));

            programFfmpegProbe.setProgram(folder.absoluteFilePath("ffprobe"));
        }
    }

    ANALYZER_RETURN
}

QFfmpegProcess::QFfmpegProcess(const QString &filename, QObject *parent):
    QObject(parent),
    programFfmpegProbe(this),
    filename(),
    xmlResProbe(),
    audioStream(),
    videoStream(),
    picture()
{
    ANALYZER

    if (EXE_DIRPATH.isEmpty())
    {
        qCritical() << "QFFmpeg, invalid installation path where are located FFMPEG binaries.";
    }
    else
    {
        QDir folder(EXE_DIRPATH);
        if (!folder.exists())
        {
            qCritical() << "QFFmpeg, invalid installation path where are located FFMPEG binaries:" << folder;
        }
        else
        {
            connect(&programFfmpegProbe, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(probeFinished(int,QProcess::ExitStatus)));

            programFfmpegProbe.setProgram(folder.absoluteFilePath("ffprobe"));

            if (setFilename(filename, true))
                programFfmpegProbe.waitForFinished();
        }
    }

    ANALYZER_RETURN
}

bool QFfmpegProcess::isValid() const
{
    return !programFfmpegProbe.program().isEmpty();
}

QFfmpegProcess::~QFfmpegProcess()
{
    ANALYZER

    if (programFfmpegProbe.state() == QProcess::Running)
    {
        programFfmpegProbe.kill();
        if (!programFfmpegProbe.waitForFinished(-1))
            qCritical() << "unable to finish probe" << filename;
    }

    ANALYZER_RETURN
}

void QFfmpegProcess::probeFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ANALYZER

    Q_UNUSED(exitCode)

    xmlResProbe.clear();
    audioStream.clear();
    videoStream.clear();

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
        qCritical() << "probe crashed" << filename;
    }

    emit mediaReady();

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

qint64 QFfmpegProcess::size() const
{
    QDomNode format = xmlResProbe.elementsByTagName("format").at(0);
    return format.attributes().namedItem("size").nodeValue().toLongLong();
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
    process.setProgram(QDir(EXE_DIRPATH).absoluteFilePath("ffmpeg"));
    QStringList arguments = QStringList() << "-i" << filename << "-f" << "mjpeg" << "-vframes" << "1" << "-s" << "300x300" << "-loglevel" << "error" << "pipe:";
    process.setArguments(arguments);
    process.start();
    if (process.waitForFinished())
    {
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
        if (process.state() == QProcess::Running)
        {
            process.kill();
            process.waitForFinished(-1);
        }
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

QHash<QString, double> QFfmpegProcess::getVolumeInfo(const int timeout)
{
    ANALYZER

    QHash<QString, double> result;

    QProcess process;
    process.setProgram(QDir(EXE_DIRPATH).absoluteFilePath("ffmpeg"));
    QStringList arguments = QStringList() << "-hide_banner" << "-nostats";
    arguments << "-i" << filename;
    arguments << "-map" << "0:a" << "-af" << "volumedetect";
    arguments << "-to" << "1800";  // parse only 30 minutes
    arguments << "-f" << "null" << "/dev/null";
    process.setArguments(arguments);
    process.start();

    if (process.waitForFinished(timeout))
    {
        if (process.exitCode() != 0)
        {
            QString error(process.readAllStandardError().trimmed());
            qCritical() << "ERROR" << process.exitCode() << filename << error;
        }
        else
        {
            QRegularExpression pattern("\\[Parsed_volumedetect_0.*\\]\\s*(\\w+):\\s*([\\+\\-\\.\\w]+)");

            QString data = process.readAllStandardError();

            QRegularExpressionMatchIterator iterator = pattern.globalMatch(data);
            while (iterator.hasNext())
            {
                QRegularExpressionMatch match = iterator.next();
                result[match.captured(1)] = match.captured(2).toDouble();
            }
        }
    }
    else
    {
        qWarning() << "NO VOLUME INFO" << filename;

        if (process.state() == QProcess::Running)
        {
            process.kill();  // timeout, process is still running
            process.waitForFinished();
        }
    }

    ANALYZER_RETURN
    return result;
}

bool QFfmpegProcess::setFilename(const QString &filename, const bool &readPicture)
{
    ANALYZER

    xmlResProbe.clear();
    audioStream.clear();
    videoStream.clear();
    picture.clear();

    QStringList argFfmpeg;
    argFfmpeg << "-i" << filename;
    argFfmpeg << "-show_format";
    argFfmpeg << "-show_entries" << "stream";
    argFfmpeg << "-of" << "xml";
    programFfmpegProbe.setArguments(argFfmpeg);

    programFfmpegProbe.start();

    if (!programFfmpegProbe.waitForStarted())
    {
        qCritical() << "probe not started" << filename;
        return false;
    }
    else
    {
        this->filename = filename;

        if (readPicture)
            picture = parsePicture();
    }

    return true;

    ANALYZER_RETURN
}
