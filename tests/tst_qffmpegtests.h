#ifndef TST_QFFMPEGTESTS_H
#define TST_QFFMPEGTESTS_H

#include <QString>
#include <QtTest>
#include "../qffmpegprocess.h"

class QFfmpegTests : public QObject
{
    Q_OBJECT

public:
    QFfmpegTests();

private Q_SLOTS:
    void testCase_Audio_MP3();
    void testCase_Audio_M4A();
    void testCase_VIDEO_MKV();
    void testCase_VIDEO_MKV2();
};

#endif // TST_QFFMPEGTESTS_H
