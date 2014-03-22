#include "tst_qffmpegtests.h"

QFfmpegTests::QFfmpegTests()
{
}

void QFfmpegTests::testCase_Audio_MP3()
{
    QString filename("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3");

    QFfmpeg media(filename, this);

    QVERIFY(media.parse_metaData() == true);

    media.dumpFormat();

    QVERIFY2(media.getFormat() == "MP2/3 (MPEG audio layer 2/3)", media.getFormat().toUtf8().constData());
    QVERIFY2(media.getStringDuration() == "00:02:39.37", media.getStringDuration().toUtf8().constData());
    QVERIFY(media.getAudioChannelCount() == 2);
    QVERIFY(media.getAudioSamplerate() == 44100);
    QVERIFY(media.getBitrate() == 192808);
    QVERIFY(media.getVideoResolution() == "500x500");
    QVERIFY(media.getVideoFrameRate() == 90000);
    QVERIFY(media.getAudioBitrate() == 189255);
    QVERIFY(media.getVideoLanguages() == QStringList() << "");
    QVERIFY(media.getAudioLanguages() == QStringList() << "");
    QVERIFY(media.getSubtitleLanguages() == QStringList());
    QVERIFY2(media.getAudioFormat() == "mp3", media.getAudioFormat().toUtf8());

    QByteArray data = media.getPicture();
    QVERIFY2(data.size() == 1592, QString("%1").arg(data.size()).toUtf8());

    QVERIFY2(media.metaData("title") == "Phébus", media.metaData("title").toUtf8().constData());
    QVERIFY2(media.metaData("artist") == "-M-", media.metaData("artist").toUtf8().constData());
    QVERIFY2(media.metaData("album") == "Mister Mystère", media.metaData("album").toUtf8().constData());
    QVERIFY2(media.metaData("track") == "2/13", media.metaData("track").toUtf8().constData());
    QVERIFY2(media.metaData("disc") == "1/2", media.metaData("disc").toUtf8().constData());
    QVERIFY2(media.metaData("genre") == "Rock  Français", media.metaData("genre").toUtf8().constData());
    QVERIFY2(media.metaData("TLEN") == "159320", media.metaData("TLEN").toUtf8().constData());
    QVERIFY2(media.metaData("date") == "2009-09-07", media.metaData("date").toUtf8().constData());
    QVERIFY2(media.metaData("unknown").isNull() == true, media.metaData("unknown").toUtf8().constData());

    QVERIFY(media.close() == true);

    ANALYZER_DISPLAY_RESULTS
}

void QFfmpegTests::testCase_Audio_M4A()
{
    QString filename("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a");

    QFfmpeg media(filename, this);

    QVERIFY(media.parse_metaData() == true);

    media.dumpFormat();

    QVERIFY2(media.getFormat() == "QuickTime / MOV", media.getFormat().toUtf8().constData());
    QVERIFY2(media.getStringDuration() == "00:03:08.98", media.getStringDuration().toUtf8().constData());
    QVERIFY(media.getAudioChannelCount() == 2);
    QVERIFY(media.getAudioSamplerate() == 44100);
    QVERIFY(media.getBitrate() == 135074);
    QVERIFY(media.getVideoResolution() == "300x300");
    QVERIFY(media.getVideoFrameRate() == 90000);
    QVERIFY(media.getAudioBitrate() == 124436);
    QVERIFY(media.getVideoLanguages() == QStringList() << "");
    QVERIFY(media.getAudioLanguages() == QStringList() << "und");
    QVERIFY(media.getSubtitleLanguages() == QStringList());
    QVERIFY2(media.getAudioFormat() == "aac", media.getAudioFormat().toUtf8());

    QByteArray data = media.getPicture();
    QVERIFY2(data.size() == 4089, QString("%1").arg(data.size()).toUtf8());

    QVERIFY2(media.metaData("title") == "Monde virtuel", media.metaData("title").toUtf8().constData());
    QVERIFY2(media.metaData("artist") == "-M-", media.metaData("artist").toUtf8().constData());
    QVERIFY2(media.metaData("album") == "Je dis aime", media.metaData("album").toUtf8().constData());
    QVERIFY2(media.metaData("track") == "1/15", media.metaData("track").toUtf8().constData());
    QVERIFY2(media.metaData("disc") == "1/1", media.metaData("disc").toUtf8().constData());
    QVERIFY2(media.metaData("genre") == "Pop", media.metaData("genre").toUtf8().constData());
    QVERIFY2(media.metaData("TLEN").isNull() == true, media.metaData("TLEN").toUtf8().constData());
    QVERIFY2(media.metaData("date") == "1999-10-25", media.metaData("date").toUtf8().constData());
    QVERIFY2(media.metaData("unknown").isNull() == true, media.metaData("unknown").toUtf8().constData());

    QVERIFY(media.close() == true);

    ANALYZER_DISPLAY_RESULTS
}

void QFfmpegTests::testCase_Audio_VIDEO()
{
    QString filename("/Users/doudou/Movies/Films/District.9.2009.720p.BrRip.YIFY.mkv");

    QFfmpeg media(filename, this);

    QVERIFY(media.parse_metaData() == true);

    media.dumpFormat();

    QVERIFY2(media.getFormat() == "Matroska / WebM", media.getFormat().toUtf8().constData());
    QVERIFY2(media.getStringDuration() == "01:52:15.83", media.getStringDuration().toUtf8().constData());
    QVERIFY(media.getAudioChannelCount() == 2);
    QVERIFY(media.getAudioSamplerate() == 48000);
    QVERIFY(media.getBitrate() == 871427);
    QVERIFY(media.getVideoResolution() == "1280x688");
    QVERIFY(QString().sprintf("%2.3f", media.getVideoFrameRate()) == "23.976");
    QVERIFY(media.getAudioBitrate() == 0);
    QVERIFY(media.getVideoLanguages() == QStringList() << "");
    QVERIFY(media.getAudioLanguages() == QStringList() << "");
    QVERIFY(media.getSubtitleLanguages() == (QStringList() << "eng"));
    QVERIFY2(media.getAudioFormat() == "aac", media.getAudioFormat().toUtf8());

    QByteArray data = media.getPicture();
    QVERIFY2(data.size() == 823, QString("%1").arg(data.size()).toUtf8());

    QVERIFY2(media.metaData("title") == "District 9 - YIFY", media.metaData("title").toUtf8().constData());

    QVERIFY(media.close() == true);

    ANALYZER_DISPLAY_RESULTS
}

void QFfmpegTests::testCase_Audio_VIDEO_MKV()
{
    QString filename("/Users/doudou/Movies/Films/Margin Call [BDrip m-1080p AC3 Esp-Eng-Fr+subs].mkv");

    QFfmpeg media(filename, this);

    QVERIFY(media.parse_metaData() == true);

    media.dumpFormat();

    QVERIFY2(media.getFormat() == "Matroska / WebM", media.getFormat().toUtf8().constData());
    QVERIFY2(media.getStringDuration() == "01:46:59.02", media.getStringDuration().toUtf8().constData());
    QVERIFY(media.getAudioChannelCount() == 6);
    QVERIFY(media.getAudioSamplerate() == 48000);
    QVERIFY(media.getBitrate() == 4293543);
    QVERIFY(media.getVideoResolution() == "1920x1080");
    QVERIFY(media.getVideoFrameRate() == 24);
    QVERIFY(media.getAudioBitrate() == 448000);
    QVERIFY(media.getVideoLanguages() == (QStringList() << "spa"));
    QVERIFY(media.getAudioLanguages() == (QStringList() << "spa" << "eng" << "fre"));
    QVERIFY(media.getSubtitleLanguages() == (QStringList() << "spa" << "eng" << "fre"));
    QVERIFY2(media.getAudioFormat() == "ac3", media.getAudioFormat().toUtf8());

    QByteArray data = media.getPicture();
    QVERIFY2(data.size() == 1073, QString("%1").arg(data.size()).toUtf8());

    QVERIFY2(media.metaData("title") == "Margin Call m-1080p.guerrero36", media.metaData("title").toUtf8().constData());

    QVERIFY(media.close() == true);

    ANALYZER_DISPLAY_RESULTS
}

QTEST_APPLESS_MAIN(QFfmpegTests)

