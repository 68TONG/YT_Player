#pragma once

#include "YT_Global.h"
#include <QQuickItem>
#include <QEasingCurve>

class YT_EXPORT YT_Easing : public QObject
{
    Q_OBJECT
        QML_SINGLETON
        QML_NAMED_ELEMENT(Easing)
private:
    explicit YT_Easing(QObject* parent = nullptr);
    ~YT_Easing();
public:
    static YT_Easing& obj();
    static YT_Easing* create(QQmlEngine*, QJSEngine*);

    Q_PROPERTY(QEasingCurve Linear READ getLinear CONSTANT)
        QEasingCurve linear;
    QEasingCurve getLinear() const;

    Q_PROPERTY(QEasingCurve Standard READ getStandard CONSTANT)
        QEasingCurve standard;
    QEasingCurve getStandard() const;

    Q_PROPERTY(QEasingCurve StandardAccelerate READ getStandardAccelerate CONSTANT)
        QEasingCurve standardAccelerate;
    QEasingCurve getStandardAccelerate() const;

    Q_PROPERTY(QEasingCurve StandardDecelerate READ getStandardDecelerate CONSTANT)
        QEasingCurve standardDecelerate;
    QEasingCurve getStandardDecelerate() const;

    Q_PROPERTY(QEasingCurve Emphasized READ getEmphasized CONSTANT)
        QEasingCurve emphasized;
    QEasingCurve getEmphasized() const;

    Q_PROPERTY(QEasingCurve EmphasizedAccelerate READ getEmphasizedAccelerate CONSTANT)
        QEasingCurve emphasizedAccelerate;
    QEasingCurve getEmphasizedAccelerate() const;

    Q_PROPERTY(QEasingCurve EmphasizedDecelerate READ getEmphasizedDecelerate CONSTANT)
        QEasingCurve emphasizedDecelerate;
    QEasingCurve getEmphasizedDecelerate() const;

    Q_PROPERTY(QEasingCurve Legacy READ getLegacy CONSTANT)
        QEasingCurve legacy;
    QEasingCurve getLegacy() const;

    Q_PROPERTY(QEasingCurve LegacyAccelerate READ getLegacyAccelerate CONSTANT)
        QEasingCurve legacyAccelerate;
    QEasingCurve getLegacyAccelerate() const;

    Q_PROPERTY(QEasingCurve LegacyDecelerate READ getLegacyDecelerate CONSTANT)
        QEasingCurve legacyDecelerate;
    QEasingCurve getLegacyDecelerate() const;

    Q_PROPERTY(QEasingCurve SpringFast READ getSpringFast CONSTANT)
        QEasingCurve springFast;
    QEasingCurve getSpringFast() const;

    Q_PROPERTY(QEasingCurve SpringDefault READ getSpringDefault  CONSTANT)
        QEasingCurve springDefault;
    QEasingCurve getSpringDefault() const;

    Q_PROPERTY(QEasingCurve SpringSlow READ getSpringSlow CONSTANT)
        QEasingCurve springSlow;
    QEasingCurve getSpringSlow() const;

    Q_PROPERTY(QEasingCurve SpringFastEx READ getSpringFastEx CONSTANT)
        QEasingCurve springFastEx;
    QEasingCurve getSpringFastEx() const;

    Q_PROPERTY(QEasingCurve SpringDefaultEx READ getSpringDefaultEx CONSTANT)
        QEasingCurve springDefaultEx;
    QEasingCurve getSpringDefaultEx() const;

    Q_PROPERTY(QEasingCurve SpringSlowEx READ getSpringSlowEx CONSTANT)
        QEasingCurve springSlowEx;
    QEasingCurve getSpringSlowEx() const;

    Q_PROPERTY(int Short1 READ getShort1 CONSTANT)
        int short1 = 50;
    int getShort1() const;

    Q_PROPERTY(int Short2 READ getShort2 CONSTANT)
        int short2 = 100;
    int getShort2() const;

    Q_PROPERTY(int Short3 READ getShort3 CONSTANT)
        int short3 = 150;
    int getShort3() const;

    Q_PROPERTY(int Short4 READ getShort4 CONSTANT)
        int short4 = 200;
    int getShort4() const;

    Q_PROPERTY(int Medium1 READ getMedium1 CONSTANT)
        int medium1 = 250;
    int getMedium1() const;

    Q_PROPERTY(int Medium2 READ getMedium2 CONSTANT)
        int medium2 = 300;
    int getMedium2() const;

    Q_PROPERTY(int Medium3 READ getMedium3 CONSTANT)
        int medium3 = 350;
    int getMedium3() const;

    Q_PROPERTY(int Medium4 READ getMedium4 CONSTANT)
        int medium4 = 400;
    int getMedium4() const;

    Q_PROPERTY(int Long1 READ getLong1 CONSTANT)
        int long1 = 450;
    int getLong1() const;

    Q_PROPERTY(int Long2 READ getLong2 CONSTANT)
        int long2 = 500;
    int getLong2() const;

    Q_PROPERTY(int Long3 READ getLong3 CONSTANT)
        int long3 = 550;
    int getLong3() const;

    Q_PROPERTY(int Long4 READ getLong4 CONSTANT)
        int long4 = 600;
    int getLong4() const;

    Q_PROPERTY(int ExtraLong1 READ getExtraLong1 CONSTANT)
        int extraLong1 = 700;
    int getExtraLong1() const;

    Q_PROPERTY(int ExtraLong2 READ getExtraLong2 CONSTANT)
        int extraLong2 = 800;
    int getExtraLong2() const;

    Q_PROPERTY(int ExtraLong3 READ getExtraLong3 CONSTANT)
        int extraLong3 = 900;
    int getExtraLong3() const;

    Q_PROPERTY(int ExtraLong4 READ getExtraLong4 CONSTANT)
        int extraLong4 = 1000;
    int getExtraLong4() const;
};
