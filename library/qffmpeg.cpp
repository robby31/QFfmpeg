#include "qffmpeg.h"

QFfmpeg::QFfmpeg(const QString &filename, QObject *parent):
    QObject(parent),
    filename(filename),
    formatContext(NULL),
    size(0),
    global_metaData(0),
    duration(-1),
    bitrate(-1),
    containerFormat(),
    audioFormat(),
    audioChannelCount(-2),
    audioSampleRate(-2),
    audioBitRate(-2),
    picture(),
    streamLanguages(0)
{
    ANALYZER

    if (!initCodec) {
        // Initialize FFmpeg
        avcodec_register_all();
        av_register_all();

        av_log_set_level(AV_LOG_QUIET);

        initCodec = true;
    }

    ANALYZER_RETURN
}

QFfmpeg::~QFfmpeg() {
    ANALYZER

    close();

    if (global_metaData != 0) {
        delete global_metaData;
    }

    if (streamLanguages != 0) {
        delete streamLanguages;
    }

    ANALYZER_RETURN
}

bool QFfmpeg::open() {
    ANALYZER

    if (initCodec) {

        if (!isOpened()) {
            // open the file
            if (avformat_open_input(&formatContext, filename.toUtf8(), NULL, NULL) != 0)
            {
                avformat_close_input(&formatContext);

                std::cout << "Error opening the file" << std::endl;
                ANALYZER_RETURN
                return isOpened();
            }

            // Retrieve stream information
            if (avformat_find_stream_info(formatContext, NULL) < 0)
            {
                avformat_close_input(&formatContext);

                std::cout << "Error finding the stream info" << std::endl;

            } else {

                if (duration == -1) {
                    // parse information and store them in object

                    // update internal data
                    duration = formatContext->duration;

                    bitrate = formatContext->bit_rate;

                    containerFormat = formatContext->iformat->long_name;

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

                    getPicture();
                }
            }
        }
    }
    ANALYZER_RETURN
    return isOpened();
}

void QFfmpeg::dumpFormat() {
    ANALYZER

    if (open()) {
        av_log_set_level(AV_LOG_VERBOSE);

        // Dump information about file onto standard error
        av_dump_format(formatContext, 0, filename.toUtf8(), 0);

        av_log_set_level(AV_LOG_QUIET);

        close();
    }
    ANALYZER_RETURN
}

int QFfmpeg::getBitrate() {
    ANALYZER

    if (bitrate == -1) {
        open();
        close();
    }

    ANALYZER_RETURN
    return bitrate;
}

QString QFfmpeg::getFormat() {
    ANALYZER
    if (containerFormat.isNull()) {
        open();
        close();
    }

    ANALYZER_RETURN
    return containerFormat;
}

QString QFfmpeg::getStringDuration() {
    ANALYZER
    if (duration == -1) {
        open();
        close();
    }

    if (duration != AV_NOPTS_VALUE) {
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

int QFfmpeg::getDuration() {
    ANALYZER
    if (duration == -1) {
        open();
        close();
    }

    if (duration != AV_NOPTS_VALUE) {
        ANALYZER_RETURN
        return int(duration / 1000);
    }

    ANALYZER_RETURN
    return -1;
}

QString QFfmpeg::getAudioFormat() {
    ANALYZER
    if (audioFormat.isNull()) {
        open();
        close();
    }

    ANALYZER_RETURN
    return audioFormat;
}

int QFfmpeg::getAudioChannelCount() {
    ANALYZER
    if (audioChannelCount == -2) {
        open();
        close();
    }

    ANALYZER_RETURN
    return audioChannelCount;
}

int QFfmpeg::getAudioSamplerate() {
    ANALYZER
    if (audioSampleRate == -2) {
        open();
        close();
    }

    ANALYZER_RETURN
    return audioSampleRate;
}

int QFfmpeg::getAudioBitrate() {
    ANALYZER
    if (audioBitRate == -2) {
        open();
        close();
    }

    ANALYZER_RETURN
    return audioBitRate;
}

AVStream* QFfmpeg::getStream(const AVMediaType &mediaType) {
    int first_stream_found = -1;

    if (!isOpened()) {
        open();
    }

    if (isOpened()) {
        // Find the video stream (some container files can have multiple streams in them)
        for (unsigned int i = 0; i < formatContext->nb_streams; ++i)
        {
            if (formatContext->streams[i]->codec->codec_type == mediaType)
            {
                if (first_stream_found == -1) {
                    first_stream_found = i;
                }

                if (formatContext->streams[i]->disposition & AV_DISPOSITION_DEFAULT) {
                    // returns the default videoStream
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

QString QFfmpeg::getVideoResolution() {
    ANALYZER
    QString res;

    AVStream* videoStream = getVideoStream();
    if (videoStream != NULL) {
        res = QString("%1x%2").arg(videoStream->codec->width).arg(videoStream->codec->height);
    }

    if (isOpened()) {
        close();
    }

    ANALYZER_RETURN
    return res;
}

double QFfmpeg::getVideoFrameRate() {
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

    if (isOpened()) {
        close();
    }

    ANALYZER_RETURN
    return res;
}

QStringList QFfmpeg::getStreamLanguages(const AVMediaType &mediaType) {
    ANALYZER
    if (streamLanguages == 0) {
        open();
        close();
    } else {
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
    }

    if (streamLanguages != 0 and streamLanguages->contains(mediaType)) {
        ANALYZER_RETURN
        return streamLanguages->value(mediaType);
    } else {
        ANALYZER_RETURN
        return QStringList();
    }
}

QByteArray QFfmpeg::getPicture() {
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

    picture = QByteArray("");

    bool flagOpened = isOpened();

    if (!isOpened())
        open();

    if (!isOpened()) {
        ANALYZER_RETURN
        return picture;
    }

    QAVFrame frame;
    if (!frame.isNull()) {

        AVStream* stream = NULL;
        int stream_index = -1;
        // Find the video stream (some container files can have multiple streams in them)
        for (unsigned int i = 0; i < formatContext->nb_streams; ++i)
        {
            if (formatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                // returns the default videoStream
                stream = formatContext->streams[i];
                stream_index = i;
                break;
            }
        }

        if (stream != NULL) {
            // Video stream found
            AVCodecContext *pCodecCtx;
            pCodecCtx = stream->codec;

            AVCodec *pCodec;

            // Find the decoder for the video stream
            pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
            if (pCodec==NULL) {
                std::cout << "Error to find codec" << std::endl;
                ANALYZER_RETURN
                return QByteArray(); // Codec not found
            }

            // Open codec
            if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
                std::cout << "Error to open codec" << std::endl;
                ANALYZER_RETURN
                return QByteArray(); // Could not open codec
            }

            if (pCodecCtx->width == 0 or pCodecCtx->height == 0) {
                avcodec_close(pCodecCtx);
                ANALYZER_RETURN
                return QByteArray();
            }

            // read frames
            int frameFinished;
            AVPacket packet;

            while (av_read_frame(formatContext, &packet) >= 0) {
                // Is this a packet from the video stream?
                if (packet.stream_index == stream_index) {
                    // Decode video frame
                    avcodec_decode_video2(pCodecCtx, frame.ptr(), &frameFinished, &packet);

                    // Did we get a video frame?
                    if (frameFinished) {
                        QImage myImage = frame.toImage(100, 100);

                        // convert Image to QByteArray
                        QBuffer img_buffer(&picture);
                        if (img_buffer.open(QIODevice::WriteOnly)) {
                            if (!myImage.save(&img_buffer, "JPEG")) {
                                picture = QByteArray();
                            }
                            img_buffer.close();
                        }
                    }
                }

                // Free the packet that was allocated by av_read_frame
                av_free_packet(&packet);

                if (frameFinished) {
                    // read only one frame
                    break;
                }
            }

            // Some codecs will cause frames to be buffered up in the decoding process. If the CODEC_CAP_DELAY flag
            // is set, there can be buffered up frames that need to be flushed, so we'll do that
            if (pCodecCtx->codec->capabilities & CODEC_CAP_DELAY)
            {
//                av_init_packet(&packet);
//                // Decode all the remaining frames in the buffer, until the end is reached
//                int frameFinished = 0;
//                while (avcodec_decode_audio4(codecContext, frame, &frameFinished, &packet) >= 0 && frameFinished)
//                {

//                }
            }

            // Close the codec
            avcodec_close(pCodecCtx);
        }
    }

    if (!flagOpened and isOpened()) {
        close();
    }

    ANALYZER_RETURN
    return picture;
}

bool QFfmpeg::decodeAudio() {
    QAVFrame frame;
    if (frame.isNull()) {
        return false;
    } else {
        // get audio stream
        AVStream* audioStream = getAudioStream();

        if (audioStream == NULL)
        {
            std::cout << "Could not find any audio stream in the file" << std::endl;
            return false;

        } else {

            AVCodecContext* codecContext = audioStream->codec;

            codecContext->codec = avcodec_find_decoder(codecContext->codec_id);
            if (codecContext->codec == NULL)
            {
                std::cout << "Couldn't find a proper decoder" << std::endl;
                return false;
            }
            else if (avcodec_open2(codecContext, codecContext->codec, NULL) != 0)
            {
                std::cout << "Couldn't open the context with the decoder" << std::endl;
                return false;
            } else {
                std::cout << "CODEC " << codecContext->codec->long_name << std::endl;
                std::cout << "This stream has " << codecContext->channels << " channels and a sample rate of " << codecContext->sample_rate << "Hz" << std::endl;
                std::cout << "The data is in the format " << av_get_sample_fmt_name(codecContext->sample_fmt) << std::endl;

                AVPacket packet;
                av_init_packet(&packet);

                // Read the packets in a loop
                while (av_read_frame(formatContext, &packet) == 0)
                {
                    if (packet.stream_index == audioStream->index)
                    {
                        // Try to decode the packet into a frame
                        int frameFinished = 0;
                        size += avcodec_decode_audio4(codecContext, frame.ptr(), &frameFinished, &packet);

                        //                std::cout << "DECODE size=" << *frame->linesize << std::endl;
                        //                size += *frame->linesize;

                        // Some frames rely on multiple packets, so we have to make sure the frame is finished before
                        // we can use it
                        if (frameFinished)
                        {
                            // frame now has usable audio data in it. How it's stored in the frame depends on the format of
                            // the audio. If it's packed audio, all the data will be in frame->data[0]. If it's in planar format,
                            // the data will be in frame->data and possibly frame->extended_data. Look at frame->data, frame->nb_samples,
                            // frame->linesize, and other related fields on the FFmpeg docs. I don't know how you're actually using
                            // the audio data, so I won't add any junk here that might confuse you. Typically, if I want to find
                            // documentation on an FFmpeg structure or function, I just type "<name> doxygen" into google (like
                            // "AVFrame doxygen" for AVFrame's docs)
                        }
                    }

                    // You *must* call av_free_packet() after each call to av_read_frame() or else you'll leak memory
                    av_free_packet(&packet);
                }

                // Some codecs will cause frames to be buffered up in the decoding process. If the CODEC_CAP_DELAY flag
                // is set, there can be buffered up frames that need to be flushed, so we'll do that
                if (codecContext->codec->capabilities & CODEC_CAP_DELAY)
                {
                    av_init_packet(&packet);
                    // Decode all the remaining frames in the buffer, until the end is reached
                    int frameFinished = 0;
                    while (avcodec_decode_audio4(codecContext, frame.ptr(), &frameFinished, &packet) >= 0 && frameFinished)
                    {
                        //                std::cout << "DELAY DECODE size=" << *frame->linesize << std::endl;
                        //                size += *frame->linesize;
                    }
                }

                std::cout << "SIZE READ = " << size << std::endl;

                return true;
            }
        }
    }
}

bool QFfmpeg::close() {
    ANALYZER
    if (isOpened()) {
        // Clean up!
        if (formatContext != NULL) {
            avformat_close_input(&formatContext);
        }
    }
    ANALYZER_RETURN
    return true;
}

bool QFfmpeg::parse_metaData() {
    ANALYZER
    // init metaData
    if (global_metaData == 0) {
        global_metaData = new QHash<QString, QString>;
    } else {
        global_metaData->clear();
    }

    if (!isOpened()) {
        // open formatContext
        open();
    }

    if (isOpened()) {
        // read all metaData
        AVDictionaryEntry *tag = NULL;
        while ((tag = av_dict_get(formatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
            global_metaData->insert(QString(tag->key).trimmed(), QString(tag->value).trimmed());
        }

        // close formatContext
        close();

        ANALYZER_RETURN
        return true;
    } else {
        ANALYZER_RETURN
        return false;
    }
}

QString QFfmpeg::metaData(const QString &tagName) {
    ANALYZER
    QString res;

    if (global_metaData == 0) {
        parse_metaData();
    }

    if (global_metaData->contains(tagName)) {
        res = global_metaData->value(tagName);
    }

    ANALYZER_RETURN
    return res;
}

