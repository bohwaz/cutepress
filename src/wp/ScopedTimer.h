#ifndef SCOPEDTIMER_H
#define SCOPEDTIMER_H

//#define DO_TIMING

#if !defined(DO_TIMING) || defined(QT_NO_DEBUG)

#define TIME_FUNCTION
#define TIME_FUNCTION_ALWAYS
#define TIME_FUNCTION_THRESHOLD(threshold)
#define TIME_CUSTOM(name, threshold)

#define TIME_SECTIONS_START
#define TIME_NEW_SECTION(name, threshold)
#define TIME_END_SECTION

#else

#include <QString>
#include <QTime>
#include <QDebug>
#include <QScopedPointer>

class ScopedTimer;

#define TIME_FUNCTION ScopedTimer scoped_timer__(__FUNCTION__);
#define TIME_FUNCTION_ALWAYS ScopedTimer scoped_timer__(__FUNCTION__, 0);
#define TIME_FUNCTION_THRESHOLD(threshold) ScopedTimer scoped_timer__(__FUNCTION__, (threshold));
#define TIME_CUSTOM(name, threshold) ScopedTimer scoped_timer__((name), (threshold));

#define TIME_SECTIONS_START QScopedPointer<ScopedTimer> scoped_timer_sections__;
#define TIME_NEW_SECTION(name, threshold) scoped_timer_sections__.reset(new ScopedTimer((name), (threshold)));
#define TIME_END_SECTION scoped_timer_sections__.reset();

class ScopedTimer
{
public:
    ScopedTimer (QString const & name, int threshold = 200)
        : name (name)
        , threshold(threshold)
    {
        timeStarted.start();
    }

    ~ScopedTimer()
    {
        int elapsed = timeStarted.elapsed();
        if(elapsed >= threshold)
        {
            qDebug() << "-=!=- WP::ScopedTimer:" << name << "took" << elapsed << "milliseconds to execute!";
        }
    }


private:
    QString name;
    int threshold;
    QTime timeStarted;
};

#endif // DO_TIMING

#endif // SCOPEDTIMER_H
