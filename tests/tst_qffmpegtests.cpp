#include "tst_qffmpegtests.h"

QFfmpegTests::QFfmpegTests()
{
    QFfmpegProcess::setDirPath("/opt/local/bin");
}

void QFfmpegTests::testCase_Audio_MP3()
{
    QString filename("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3");

    QFfmpegProcess media(filename, this);

    QVERIFY2(media.getFormat() == "mp3", media.getFormat().toUtf8().constData());
    QVERIFY2(media.getDuration() == 159373, QString("%1").arg(media.getDuration()).toUtf8());
    QVERIFY(media.getAudioChannelCount() == 2);
    QVERIFY(media.getAudioSamplerate() == 44100);
    QVERIFY(media.getBitrate() == 192808);
    QVERIFY(media.getVideoResolution() == "500x500");
    QVERIFY2(media.getVideoFrameRate() == 0.0, QString("%1").arg(media.getVideoFrameRate()).toUtf8());
    QVERIFY(media.getAudioBitrate() == 189255);
    QVERIFY(media.getVideoLanguages() == QStringList() << "");
    QVERIFY(media.getAudioLanguages() == QStringList() << "");
    QVERIFY(media.getSubtitleLanguages() == QStringList());
    QVERIFY2(media.getAudioFormat() == "mp3", media.getAudioFormat().toUtf8());

    QByteArray data = media.getPicture();
    QVERIFY2(data.size() == 5290, QString("%1").arg(data.size()).toUtf8());

    QVERIFY2(media.metaData("title") == "Phébus", media.metaData("title").toUtf8().constData());
    QVERIFY2(media.metaData("artist") == "-M-", media.metaData("artist").toUtf8().constData());
    QVERIFY2(media.metaData("album") == "Mister Mystère", media.metaData("album").toUtf8().constData());
    QVERIFY2(media.metaData("track") == "2/13", media.metaData("track").toUtf8().constData());
    QVERIFY2(media.metaData("disc") == "1/2", media.metaData("disc").toUtf8().constData());
    QVERIFY2(media.metaData("genre") == "Rock  Français", media.metaData("genre").toUtf8().constData());
    QVERIFY2(media.metaData("TLEN") == "159320", media.metaData("TLEN").toUtf8().constData());
    QVERIFY2(media.metaData("date") == "2009-09-07", media.metaData("date").toUtf8().constData());
    QVERIFY2(media.metaData("unknown").isNull() == true, media.metaData("unknown").toUtf8().constData());

    QFfmpegProcess media2("/Users/doudou/Music/iTunes/iTunes Media/Music/BB Brunes/Nico Teen Love/02 Dynamite.mp3", this);

    data = media2.getPicture();
    QVERIFY2(data.size() == 19021, QString("%1").arg(data.size()).toUtf8());

    ANALYZER_DISPLAY_RESULTS
}

void QFfmpegTests::testCase_Audio_M4A()
{
    QString filename("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a");

    QFfmpegProcess media(filename, this);

    QVERIFY2(media.getFormat() == "mov,mp4,m4a,3gp,3g2,mj2", media.getFormat().toUtf8().constData());
    QVERIFY2(media.getDuration() == 188987, QString("%1").arg(media.getDuration()).toUtf8());
    QVERIFY(media.getAudioChannelCount() == 2);
    QVERIFY(media.getAudioSamplerate() == 44100);
    QVERIFY(media.getBitrate() == 135074);
    QVERIFY(media.getVideoResolution() == "300x300");
    QVERIFY2(media.getVideoFrameRate() == 0.0, QString("%1").arg(media.getVideoFrameRate()).toUtf8());
    QVERIFY(media.getAudioBitrate() == 124436);
    QVERIFY(media.getVideoLanguages() == QStringList() << "");
    QVERIFY(media.getAudioLanguages() == QStringList() << "und");
    QVERIFY(media.getSubtitleLanguages() == QStringList());
    QVERIFY2(media.getAudioFormat() == "aac", media.getAudioFormat().toUtf8());

    QByteArray data = media.getPicture();
    QVERIFY2(data.size() == 21247, QString("%1").arg(data.size()).toUtf8());

    QVERIFY2(media.metaData("title") == "Monde virtuel", media.metaData("title").toUtf8().constData());
    QVERIFY2(media.metaData("artist") == "-M-", media.metaData("artist").toUtf8().constData());
    QVERIFY2(media.metaData("album") == "Je dis aime", media.metaData("album").toUtf8().constData());
    QVERIFY2(media.metaData("track") == "1/15", media.metaData("track").toUtf8().constData());
    QVERIFY2(media.metaData("disc") == "1/1", media.metaData("disc").toUtf8().constData());
    QVERIFY2(media.metaData("genre") == "Pop", media.metaData("genre").toUtf8().constData());
    QVERIFY2(media.metaData("TLEN").isNull() == true, media.metaData("TLEN").toUtf8().constData());
    QVERIFY2(media.metaData("date") == "1999-10-25", media.metaData("date").toUtf8().constData());
    QVERIFY2(media.metaData("unknown").isNull() == true, media.metaData("unknown").toUtf8().constData());

    ANALYZER_DISPLAY_RESULTS
}

void QFfmpegTests::testCase_VIDEO_MKV()
{
    QString filename("/Users/doudou/Movies/Films/District.9.2009.720p.BrRip.YIFY.mkv");

    QFfmpegProcess media(filename, this);

    QVERIFY2(media.getFormat() == "matroska,webm", media.getFormat().toUtf8().constData());
    QVERIFY2(media.getDuration() == 6735830, QString("%1").arg(media.getDuration()).toUtf8());
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
    QVERIFY2(data.size() == 2008, QString("%1").arg(data.size()).toUtf8());

    QVERIFY2(media.metaData("title") == "District 9 - YIFY", media.metaData("title").toUtf8().constData());

    ANALYZER_DISPLAY_RESULTS
}

void QFfmpegTests::testCase_VIDEO_MKV2()
{
    QString filename("/Users/doudou/Movies/Films/Margin Call [BDrip m-1080p AC3 Esp-Eng-Fr+subs].mkv");

    QFfmpegProcess media(filename, this);

    QVERIFY2(media.getFormat() == "matroska,webm", media.getFormat().toUtf8().constData());
    QVERIFY2(media.getDuration() == 6419022, QString("%1").arg(media.getDuration()).toUtf8());
    QVERIFY(media.getAudioChannelCount() == 2);
    QVERIFY(media.getAudioSamplerate() == 48000);
    QVERIFY(media.getBitrate() == 4293543);
    QVERIFY(media.getVideoResolution() == "1920x1080");
    QVERIFY(media.getVideoFrameRate() == 24);
    QVERIFY2(media.getAudioBitrate() == 112000, QString("%1").arg(media.getAudioBitrate()).toUtf8());
    QVERIFY(media.getVideoLanguages() == (QStringList() << "spa"));
    QVERIFY(media.getAudioLanguages() == (QStringList() << "spa" << "eng" << "fre"));
    QVERIFY(media.getSubtitleLanguages() == (QStringList() << "spa" << "eng" << "fre"));
    QVERIFY2(media.getAudioFormat() == "ac3", media.getAudioFormat().toUtf8());

    QByteArray data = media.getPicture();
    QVERIFY2(data.size() == 4054, QString("%1").arg(data.size()).toUtf8());

    QVERIFY2(media.metaData("title") == "Margin Call m-1080p.guerrero36", media.metaData("title").toUtf8().constData());

    ANALYZER_DISPLAY_RESULTS
}

QTEST_APPLESS_MAIN(QFfmpegTests)

