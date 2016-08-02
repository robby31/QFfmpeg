#include "qffmpeg.h"

QFfmpeg::QFfmpeg(const QString &filename, QObject *parent):
    QObject(parent),
    filename(filename),
    formatContext(NULL),
    frame(this),
    size(0),
    audioFormat(),
    audioChannelCount(-2),
    audioSampleRate(-2),
    audioBitRate(-2),
    picture(),
    streamLanguages(0),
    resultStreamInfoParsed(1)  //findStreamInfo not called
{
    ANALYZER

    if (!initCodec) {
        // Initialize FFmpeg
        avcodec_register_all();
        av_register_all();

        av_log_set_level(AV_LOG_QUIET);

        initCodec = true;
    }

    open();

    picture = parsePictureByProcess();
//    picture = parsePictureByApi();

    ANALYZER_RETURN
}

QFfmpeg::~QFfmpeg() {
    ANALYZER

    close();

    if (streamLanguages != 0)
        delete streamLanguages;

    ANALYZER_RETURN
}

bool QFfmpeg::open() {
    ANALYZER

    if (initCodec) {
        if (!isOpened()) {
            // open the file
            if (avformat_open_input(&formatContext, filename.toUtf8(), NULL, NULL) != 0)
            {
                std::cout << "Error opening the file" << std::endl;
                ANALYZER_RETURN
                return isOpened();
            }
            
            // Retrieve stream information
            findStreamInfo();

            if (audioBitRate == -2) {
                // parse information and store them in object

                // update internal data
                if (streamLanguages == 0) {
                    streamLanguages = new QHash<AVMediaType, QStringList>;
                } else {
                    streamLanguages->clear();
                }
                streamLanguages->insert(AVMEDIA_TYPE_AUDIO, getStreamLanguages(AVMEDIA_TYPE_AUDIO));
                streamLanguages->insert(AVMEDIA_TYPE_SUBTITLE, getStreamLanguages(AVMEDIA_TYPE_SUBTITLE));
                streamLanguages->insert(AVMEDIA_TYPE_VIDEO, getStreamLanguages(AVMEDIA_TYPE_VIDEO));

                audioFormat = QString("");
                audioChannelCount = -1;
                audioSampleRate = -1;
                audioBitRate = -1;
                AVStream* audioStream = getAudioStream();
                if (audioStream != NULL) {
                    AVCodecContext* audioCodecContext = audioStream->codec;

                    audioCodecContext->codec = avcodec_find_decoder(audioCodecContext->codec_id);
                    if (audioCodecContext->codec != NULL)
                    {
                        if (avcodec_open2(audioCodecContext, audioCodecContext->codec, NULL) == 0)
                        {
                            audioFormat = audioCodecContext->codec->name;
                            audioChannelCount = audioCodecContext->channels;
                            audioSampleRate = audioCodecContext->sample_rate;
                            audioBitRate = audioCodecContext->bit_rate;

                            // Close the codec
                            avcodec_close(audioCodecContext);
                        }
                    }
                }

//                getPicture();
            }
        }
    }
    ANALYZER_RETURN
    return isOpened();
}

int QFfmpeg::findStreamInfo() {
    if (resultStreamInfoParsed==1)
        resultStreamInfoParsed = avformat_find_stream_info(formatContext, NULL);
    if (resultStreamInfoParsed<0)
        std::cout << "Error finding the stream info" << std::endl;
    return resultStreamInfoParsed;
}

int QFfmpeg::openCodecContext(const AVMediaType &mediaType)
{
    int ret = -1;

    ret = av_find_best_stream(formatContext, mediaType, -1, -1, NULL, 0);
    if (ret < 0) {
        std::cerr << QString("Could not find %1 stream in input file '%2'").arg(av_get_media_type_string(mediaType)).arg(filename).toUtf8().constData() << std::endl;
        return ret;
    }

    // index of the stream
    uint stream_idx = uint(ret);

    return openCodecContext(stream_idx);
}

int QFfmpeg::openCodecContext(const uint &stream_idx)
{
    int ret = -1;

    if (stream_idx < formatContext->nb_streams) {
        AVCodecContext *dec_ctx = formatContext->streams[stream_idx]->codec;

        if (avcodec_is_open(dec_ctx)) {
            // decoder for the stream is already opened
            return 0;
        }

        // find decoder for the stream
        AVCodec *dec = avcodec_find_decoder(dec_ctx->codec_id);
        if (!dec) {
            std::cerr << QString("Failed to find %1 codec").arg(dec_ctx->codec_id).toUtf8().constData() << std::endl;
            return -1;
        }

        // open the decoder for the stream
        if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
            std::cerr << QString("Failed to open %1 codec").arg(dec_ctx->codec_id).toUtf8().constData() << std::endl;
            return -1;
        }

        return 0;
    }

    return -1;
}

void QFfmpeg::dumpFormat() {
    ANALYZER

    if (!isOpened())
        open();

    if (isOpened()) {
        av_log_set_level(AV_LOG_VERBOSE);

        // Dump information about file onto standard error
        av_dump_format(formatContext, 0, filename.toUtf8(), 0);

        av_log_set_level(AV_LOG_QUIET);
    }

    ANALYZER_RETURN
}

int QFfmpeg::getBitrate() const {
    ANALYZER

    int bitrate = -1;

    if (isOpened())
        bitrate = formatContext->bit_rate;

    ANALYZER_RETURN
    return bitrate;
}

QString QFfmpeg::getFormat() const {
    ANALYZER

    QString format;

    if (isOpened())
        format = formatContext->iformat->long_name;

    ANALYZER_RETURN
    return format;
}

QString QFfmpeg::getStringDuration() const {
    ANALYZER

    int64_t duration = getDuration();

    if (duration != AV_NOPTS_VALUE) {
        duration = duration * 1000;

        int hours, mins, secs, us;
        secs = duration / AV_TIME_BASE;
        us = duration % AV_TIME_BASE;
        mins = secs / 60;
        secs %= 60;
        hours = mins / 60;
        mins %= 60;
        QString res;
        res.sprintf("%02d:%02d:%02d.%02d", hours, mins, secs, (100 * us) / AV_TIME_BASE);
        ANALYZER_RETURN
        return res;
    }

    ANALYZER_RETURN
    return QString();
}

int64_t QFfmpeg::getDuration() const {
    ANALYZER

    int64_t duration = AV_NOPTS_VALUE;

    if (isOpened())
        duration = formatContext->duration;

    if (duration != AV_NOPTS_VALUE)
        duration = duration / 1000;

    ANALYZER_RETURN
    return duration;
}

QString QFfmpeg::getAudioFormat() const {
    ANALYZER

    ANALYZER_RETURN
    return audioFormat;
}

int QFfmpeg::getAudioChannelCount() const {
    ANALYZER

    ANALYZER_RETURN
    return audioChannelCount;
}

int QFfmpeg::getAudioSamplerate() const {
    ANALYZER

    ANALYZER_RETURN
    return audioSampleRate;
}

int QFfmpeg::getAudioBitrate() const {
    ANALYZER

    ANALYZER_RETURN
    return audioBitRate;
}

AVStream* QFfmpeg::getStream(const AVMediaType &mediaType) const {
    int first_stream_found = -1;

    if (isOpened()) {
        // Find the stream
        for (unsigned int i = 0; i < formatContext->nb_streams; ++i)
        {
            if (formatContext->streams[i]->codec->codec_type == mediaType)
            {
                if (first_stream_found == -1) {
                    first_stream_found = i;
                }

                if (formatContext->streams[i]->disposition & AV_DISPOSITION_DEFAULT) {
                    // returns the default stream
                    return formatContext->streams[i];
                }
            }
        }

        if (first_stream_found != -1) {
            // No default stream has been found so returns the first one
            return formatContext->streams[first_stream_found];
        }
    }

    return NULL;
}

QString QFfmpeg::getVideoResolution() const {
    ANALYZER
    QString res;

    AVStream* videoStream = getVideoStream();
    if (videoStream != NULL) {
        res = QString("%1x%2").arg(videoStream->codec->width).arg(videoStream->codec->height);
    }

    ANALYZER_RETURN
    return res;
}

double QFfmpeg::getVideoFrameRate() const {
    ANALYZER
    double res = 0.0;

    AVStream* videoStream = getVideoStream();
    if (videoStream != NULL) {
        if (videoStream->avg_frame_rate.den && videoStream->avg_frame_rate.num) {
            res = av_q2d(videoStream->avg_frame_rate);
        } else if (videoStream->r_frame_rate.den && videoStream->r_frame_rate.num) {
            res = av_q2d(videoStream->r_frame_rate);
        } else if (videoStream->time_base.den && videoStream->time_base.num) {
            res = 1/av_q2d(videoStream->time_base);
        } else if (videoStream->codec->time_base.den && videoStream->codec->time_base.num) {
            res = 1/av_q2d(videoStream->codec->time_base);
        }
    }

    ANALYZER_RETURN
    return res;
}

QStringList QFfmpeg::getStreamLanguages(const AVMediaType &mediaType) const {
    ANALYZER

    if (isOpened()) {
        QStringList languages;
        // Parse the streams
        for (unsigned int i = 0; i < formatContext->nb_streams; ++i)
        {
            if (formatContext->streams[i]->codec->codec_type == mediaType)
            {
                AVStream* subtitleStream = formatContext->streams[i];
                AVDictionaryEntry *lang = av_dict_get(subtitleStream->metadata, "language", NULL, 0);
                if (lang != NULL) {
                    languages << lang->value;
                } else {
                    languages << "";
                }
            }
        }
        streamLanguages->insert(mediaType, languages);
    }

    if (streamLanguages != 0 and streamLanguages->contains(mediaType)) {
        ANALYZER_RETURN
        return streamLanguages->value(mediaType);
    } else {
        ANALYZER_RETURN
        return QStringList();
    }
}

QByteArray QFfmpeg::getPicture() const {
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

QByteArray QFfmpeg::parsePictureByProcess() const
{
    ANALYZER

    QByteArray picture("");

    QProcess process;
    QString program = "/Users/doudou/workspaceQT/DLNA_server/exe/ffmpeg";
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

QByteArray QFfmpeg::parsePictureByApi()
{
    ANALYZER

    QByteArray picture("");

    bool flagOpened = isOpened();

    if (!isOpened())
        open();

    if (!isOpened()) {
        ANALYZER_RETURN
        return picture;
    }

    AVStream* stream = getVideoStream();

    if (stream != NULL) {
        // Video stream found

        if (openCodecContext(stream->index) < 0) {
            std::cout << "Error to open codec" << std::endl;
            ANALYZER_RETURN
            return QByteArray(); // Could not open codec
        }

        if (stream->codec->width == 0 or stream->codec->height == 0) {
            qWarning() << "PICTURE (invalid size)" << filename;
            ANALYZER_RETURN
            return QByteArray();
        }

        // read frame
        AVPacket packet;
        av_init_packet(&packet);
        packet.data = NULL;
        packet.size = 0;

        bool got_frame = false;
        while (av_read_frame(formatContext, &packet) >= 0) {
            // go to first packet for the stream
            if (packet.stream_index == stream->index) {

                while (packet.size > 0) {
                    got_frame = decodePacket(&packet);
                    if (got_frame) {
                        picture = frame.toByteArray(300, 300);
                        break;
                    }
                }

                // Free the packet that was allocated by av_read_frame
                av_free_packet(&packet);

                if (got_frame) {
                    break;
                }
            } else {
                // Free the packet that was allocated by av_read_frame
                av_free_packet(&packet);
            }
        }

        if (picture.isEmpty())
            qWarning() << "NO PICTURE" << filename;
    }

    if (!flagOpened and isOpened()) {
        close();
    }

    ANALYZER_RETURN
    return picture;
}

bool QFfmpeg::decodePacket(AVPacket *pkt)
{
    int got_frame = 0;
    int ret = 0;

    if (frame.isNull()) {
        return false;
    }

    AVCodecContext *dec_ctx = formatContext->streams[pkt->stream_index]->codec;

    if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
        /* decode video frame */
        ret = avcodec_decode_video2(dec_ctx, frame.ptr(), &got_frame, pkt);
        if (ret < 0) {
            fprintf(stderr, "Error decoding video frame\n");
            return false;
        }

    } else if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        /* decode audio frame */
        ret = avcodec_decode_audio4(dec_ctx, frame.ptr(), &got_frame, pkt);
        if (ret < 0) {
            fprintf(stderr, "Error decoding audio frame\n");
            return false;
        }
        /* Some audio decoders decode only part of the packet, and have to be
             * called again with the remainder of the packet data.
             * Sample: fate-suite/lossless-audio/luckynight-partial.shn
             * Also, some decoders might over-read the packet. */
        ret = FFMIN(ret, pkt->size);

    } else {
        // type not supported
        return false;
    }

    pkt->data += ret;
    pkt->size -= ret;

    if (got_frame)
        return true;

    return false;
}

int QFfmpeg::decodeStream(uint stream_idx) {
    int nbDecodedFrames = 0;

    // go to the beginning of the file
    if (isOpened())
        close();
    open();

    if (openCodecContext(stream_idx) != 0)
        return -1;

    /* initialize packet, set data to NULL, let the demuxer fill it */
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    /* read frames from the file */
    while (av_read_frame(formatContext, &pkt) >= 0) {
        AVPacket orig_pkt = pkt;
        if (uint(pkt.stream_index) == stream_idx) {
            while (pkt.size > 0) {
                if (decodePacket(&pkt)) {
                    nbDecodedFrames++;
                }
            }
        }
        av_free_packet(&orig_pkt);
    }

    if (uint(pkt.stream_index) == stream_idx) {
        /* flush cached frames */
        pkt.data = NULL;
        pkt.size = 0;
        while (decodePacket(&pkt))
            nbDecodedFrames++;
    }

    return nbDecodedFrames;
}


bool QFfmpeg::close() {
    ANALYZER
    if (isOpened()) {
        // Clean up!
        for (uint index=0;index<formatContext->nb_streams;index++)
            if (avcodec_is_open(formatContext->streams[index]->codec)) {
                avcodec_close(formatContext->streams[index]->codec);
            }
        avformat_close_input(&formatContext);
    }
    resultStreamInfoParsed = 1;
    ANALYZER_RETURN
    return true;
}

QString QFfmpeg::metaData(const QString &tagName) const {
    ANALYZER
    QString res;

    if (isOpened()) {
        AVDictionaryEntry *tag = NULL;
        tag = av_dict_get(formatContext->metadata, tagName.toUtf8().constData(), NULL, AV_DICT_IGNORE_SUFFIX);
        if (tag != NULL)
            res = QString(tag->value).trimmed();
    }

    ANALYZER_RETURN
    return res;
}

