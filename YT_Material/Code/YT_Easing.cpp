#include "YT_Easing.h"

#include <QQmlEngine>
#include <QJSEngine>

YT_Easing::YT_Easing(QObject* parent)
    : QObject{ parent }
{
    linear = QEasingCurve(QEasingCurve::Linear);

    standard = QEasingCurve(QEasingCurve::BezierSpline);
    standard.addCubicBezierSegment({ 0.2, 0.0 }, { 0.0, 1.0 }, { 1.0, 1.0 });

    standardAccelerate = QEasingCurve(QEasingCurve::BezierSpline);
    standardAccelerate.addCubicBezierSegment({ 0.3, 0.0 }, { 1.0, 1.0 }, { 1.0, 1.0 });

    standardDecelerate = QEasingCurve(QEasingCurve::BezierSpline);
    standardDecelerate.addCubicBezierSegment({ 0.0, 0.0 }, { 0.0, 1.0 }, { 1.0, 1.0 });

    emphasized = QEasingCurve(QEasingCurve::BezierSpline);
    emphasized.addCubicBezierSegment({ 0.05, 0.0 }, { 0.133333, 0.06 }, { 0.166666, 0.4 });
    emphasized.addCubicBezierSegment({ 0.208333, 0.82 }, { 0.25, 1.0 }, { 1.0, 1.0 });

    emphasizedAccelerate = QEasingCurve(QEasingCurve::BezierSpline);
    emphasizedAccelerate.addCubicBezierSegment({ 0.3, 0.0 }, { 0.8, 0.15 }, { 1.0, 1.0 });

    emphasizedDecelerate = QEasingCurve(QEasingCurve::BezierSpline);
    emphasizedDecelerate.addCubicBezierSegment({ 0.05, 0.7 }, { 0.1, 1.0 }, { 1.0, 1.0 });

    legacy = QEasingCurve(QEasingCurve::BezierSpline);
    legacy.addCubicBezierSegment({ 0.4, 0.0 }, { 0.2, 1.0 }, { 1.0, 1.0 });

    legacyAccelerate = QEasingCurve(QEasingCurve::BezierSpline);
    legacyAccelerate.addCubicBezierSegment({ 0.4, 0.0 }, { 1.0, 1.0 }, { 1.0, 1.0 });

    legacyDecelerate = QEasingCurve(QEasingCurve::BezierSpline);
    legacyDecelerate.addCubicBezierSegment({ 0.0, 0.0 }, { 0.2, 1.0 }, { 1.0, 1.0 });

    springFast = QEasingCurve(QEasingCurve::BezierSpline);
    springFast.addCubicBezierSegment({ 0.27, 1.06 }, { 0.18, 1.00 }, { 1.0, 1.0 });

    springDefault = QEasingCurve(QEasingCurve::BezierSpline);
    springDefault.addCubicBezierSegment({ 0.27, 1.06 }, { 0.18, 1.00 }, { 1.0, 1.0 });

    springSlow = QEasingCurve(QEasingCurve::BezierSpline);
    springSlow.addCubicBezierSegment({ 0.27, 1.06 }, { 0.18, 1.00 }, { 1.0, 1.0 });

    springFastEx = QEasingCurve(QEasingCurve::BezierSpline);
    springFastEx.addCubicBezierSegment({ 0.42, 1.67 }, { 0.21, 0.90 }, { 1.0, 1.0 });

    springDefaultEx = QEasingCurve(QEasingCurve::BezierSpline);
    springDefaultEx.addCubicBezierSegment({ 0.38, 1.21 }, { 0.22, 1.00 }, { 1.0, 1.0 });

    springSlowEx = QEasingCurve(QEasingCurve::BezierSpline);
    springSlowEx.addCubicBezierSegment({ 0.39, 1.29 }, { 0.35, 0.98 }, { 1.0, 1.0 });

}

YT_Easing::~YT_Easing()
{
    {}
}

YT_Easing& YT_Easing::obj()
{
    static YT_Easing object;
    return object;
}

YT_Easing* YT_Easing::create(QQmlEngine*, QJSEngine*)
{
    QJSEngine::setObjectOwnership(&obj(), QJSEngine::CppOwnership);
    return &obj();
}

QEasingCurve YT_Easing::getLinear() const
{
    return linear;
}

QEasingCurve YT_Easing::getStandard() const
{
    return standard;
}

QEasingCurve YT_Easing::getStandardAccelerate() const
{
    return standardAccelerate;
}

QEasingCurve YT_Easing::getStandardDecelerate() const
{
    return standardDecelerate;
}

QEasingCurve YT_Easing::getEmphasized() const
{
    return emphasized;
}

QEasingCurve YT_Easing::getEmphasizedAccelerate() const
{
    return emphasizedAccelerate;
}

QEasingCurve YT_Easing::getEmphasizedDecelerate() const
{
    return emphasizedDecelerate;
}

QEasingCurve YT_Easing::getLegacy() const
{
    return legacy;
}

QEasingCurve YT_Easing::getLegacyAccelerate() const
{
    return legacyAccelerate;
}

QEasingCurve YT_Easing::getLegacyDecelerate() const
{
    return legacyDecelerate;
}

QEasingCurve YT_Easing::getSpringFast() const
{
    return springFast;
}

QEasingCurve YT_Easing::getSpringDefault() const
{
    return springDefault;
}

QEasingCurve YT_Easing::getSpringSlow() const
{
    return springSlow;
}

QEasingCurve YT_Easing::getSpringFastEx() const
{
    return springFastEx;
}

QEasingCurve YT_Easing::getSpringDefaultEx() const
{
    return springDefaultEx;
}

QEasingCurve YT_Easing::getSpringSlowEx() const
{
    return springSlowEx;
}

int YT_Easing::getShort1() const
{
    return short1;
}

int YT_Easing::getShort2() const
{
    return short2;
}

int YT_Easing::getShort3() const
{
    return short3;
}

int YT_Easing::getShort4() const
{
    return short4;
}

int YT_Easing::getMedium1() const
{
    return medium1;
}

int YT_Easing::getMedium2() const
{
    return medium2;
}

int YT_Easing::getMedium3() const
{
    return medium3;
}

int YT_Easing::getMedium4() const
{
    return medium4;
}

int YT_Easing::getLong1() const
{
    return long1;
}

int YT_Easing::getLong2() const
{
    return long2;
}

int YT_Easing::getLong3() const
{
    return long3;
}

int YT_Easing::getLong4() const
{
    return long4;
}

int YT_Easing::getExtraLong1() const
{
    return extraLong1;
}

int YT_Easing::getExtraLong2() const
{
    return extraLong2;
}

int YT_Easing::getExtraLong3() const
{
    return extraLong3;
}

int YT_Easing::getExtraLong4() const
{
    return extraLong4;
}
