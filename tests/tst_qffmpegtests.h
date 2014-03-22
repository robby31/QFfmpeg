#ifndef TST_QFFMPEGTESTS_H
#define TST_QFFMPEGTESTS_H

#include <QString>
#include <QtTest>
#include "../library/qffmpeg.h"

class QFfmpegTests : public QObject
{
    Q_OBJECT

public:
    QFfmpegTests();

private Q_SLOTS:
    void testCase_Audio_MP3();
    void testCase_Audio_M4A();
    void testCase_Audio_VIDEO();
    void testCase_Audio_VIDEO_MKV();
};

#endif // TST_QFFMPEGTESTS_H
