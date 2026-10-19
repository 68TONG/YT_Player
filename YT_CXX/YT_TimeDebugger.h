#pragma once

#include <QDebug>
#include <QElapsedTimer>
#include <QString>

class YT_TimeDebugger
{
public:
    enum TimeUnit
    {
        Seconds,
        Milli,
        Micro,
        Nano
    };

    YT_TimeDebugger() noexcept
        : unit_(Milli), auto_print_(false)
    {
        start_.start();
        last_.start();
    }

    YT_TimeDebugger(TimeUnit unit) noexcept
        : name_(QStringLiteral("YT_TimeDebugger")), unit_(unit), auto_print_(true)
    {
        start_.start();
        last_.start();
        qInfo().noquote() << QStringLiteral("[%1]").arg(name_);
    }

    YT_TimeDebugger(const QString& name, TimeUnit unit = Milli) noexcept
        : name_(name), unit_(unit), auto_print_(true)
    {
        start_.start();
        last_.start();
        qInfo().noquote() << QStringLiteral("[%1]").arg(name_);
    }

    ~YT_TimeDebugger() noexcept
    {
        if (auto_print_)
        {
            callElapsedSince(QStringLiteral("~YT_TimeDebugger"));
        }
    }

    double elapsedSeconds(const QString& msg = {}) const noexcept
    {
        return elapsed(1e9, QStringLiteral("s"), msg);
    }

    double elapsedMilli(const QString& msg = {}) const noexcept
    {
        return elapsed(1e6, QStringLiteral("ms"), msg);
    }

    double elapsedMicro(const QString& msg = {}) const noexcept
    {
        return elapsed(1e3, QStringLiteral("μs"), msg);
    }

    double elapsedNano(const QString& msg = {}) const noexcept
    {
        return elapsed(1.0, QStringLiteral("ns"), msg);
    }

    double elapsedSinceSeconds(const QString& msg = {}) noexcept
    {
        return elapsedSince(1e9, QStringLiteral("s"), msg);
    }

    double elapsedSinceMilli(const QString& msg = {}) noexcept
    {
        return elapsedSince(1e6, QStringLiteral("ms"), msg);
    }

    double elapsedSinceMicro(const QString& msg = {}) noexcept
    {
        return elapsedSince(1e3, QStringLiteral("μs"), msg);
    }

    double elapsedSinceNano(const QString& msg = {}) noexcept
    {
        return elapsedSince(1.0, QStringLiteral("ns"), msg);
    }

    YT_TimeDebugger& operator<<(const QString& msg) noexcept
    {
        callElapsedSince(msg);
        return *this;
    }

    void reset() noexcept
    {
        start_.restart();
        last_.restart();
    }

private:
    QElapsedTimer start_;
    mutable QElapsedTimer last_;
    QString name_;
    TimeUnit unit_;
    bool auto_print_;

    void callElapsedSince(const QString& msg) noexcept
    {
        switch (unit_)
        {
        case TimeUnit::Seconds:
            elapsedSinceSeconds(msg);
            break;
        case TimeUnit::Milli:
            elapsedSinceMilli(msg);
            break;
        case TimeUnit::Micro:
            elapsedSinceMicro(msg);
            break;
        case TimeUnit::Nano:
            elapsedSinceNano(msg);
            break;
        }
    }

    double elapsed(double nanoseconds_per_unit, const QString& unit, const QString& msg) const noexcept
    {
        const double value = static_cast<double>(start_.nsecsElapsed()) / nanoseconds_per_unit;
        last_.restart();
        if (!msg.isEmpty())
        {
            printMsg(msg, value, unit);
        }
        return value;
    }

    double elapsedSince(double nanoseconds_per_unit, const QString& unit, const QString& msg) noexcept
    {
        const double value = static_cast<double>(last_.nsecsElapsed()) / nanoseconds_per_unit;
        last_.restart();
        printMsg(msg, value, unit);
        return value;
    }

    void printMsg(const QString& msg, double value, const QString& unit) const noexcept
    {
        const QString prefix = name_.isEmpty() ? QString{} : name_ + QStringLiteral("] [");
        qInfo().noquote() << QStringLiteral("[%1%2] %3 %4")
                                 .arg(prefix)
                                 .arg(msg)
                                 .arg(value, 0, 'g', 6)
                                 .arg(unit);
    }
};
