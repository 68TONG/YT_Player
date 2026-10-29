#pragma once

#include "YT_Global.h"
#include <QFont>
#include <QObject>
#include <QJSEngine>
#include <QQmlEngine>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

class YT_EXPORT YT_Font : public QObject
{
    Q_OBJECT
        QML_SINGLETON
        QML_NAMED_ELEMENT(Font)
private:
    explicit YT_Font(QObject* parent = nullptr);
    ~YT_Font() = default;
public:
    static YT_Font& obj();
    static YT_Font* create(QQmlEngine*, QJSEngine*);

    Q_INVOKABLE QFont createFont(
        const QString& family,
        qreal pixelSize,
        int weight,
        qreal letterSpacing
    ) const;
    Q_INVOKABLE QVariantList getVariableAxes(const QFont& font) const;
    Q_PROPERTY(QVariantList VariableAxes READ getRobotoVariableAxes CONSTANT)
    QVariantList getRobotoVariableAxes() const;

    Q_PROPERTY(QFont DisplayLarge MEMBER displayLarge CONSTANT)
    Q_PROPERTY(qreal DisplayLargeLineHeight MEMBER displayLargeLineHeight CONSTANT)
    Q_PROPERTY(QFont DisplayMedium MEMBER displayMedium CONSTANT)
    Q_PROPERTY(qreal DisplayMediumLineHeight MEMBER displayMediumLineHeight CONSTANT)
    Q_PROPERTY(QFont DisplaySmall MEMBER displaySmall CONSTANT)
    Q_PROPERTY(qreal DisplaySmallLineHeight MEMBER displaySmallLineHeight CONSTANT)
    Q_PROPERTY(QFont HeadlineLarge MEMBER headlineLarge CONSTANT)
    Q_PROPERTY(qreal HeadlineLargeLineHeight MEMBER headlineLargeLineHeight CONSTANT)
    Q_PROPERTY(QFont HeadlineMedium MEMBER headlineMedium CONSTANT)
    Q_PROPERTY(qreal HeadlineMediumLineHeight MEMBER headlineMediumLineHeight CONSTANT)
    Q_PROPERTY(QFont HeadlineSmall MEMBER headlineSmall CONSTANT)
    Q_PROPERTY(qreal HeadlineSmallLineHeight MEMBER headlineSmallLineHeight CONSTANT)
    Q_PROPERTY(QFont TitleLarge MEMBER titleLarge CONSTANT)
    Q_PROPERTY(qreal TitleLargeLineHeight MEMBER titleLargeLineHeight CONSTANT)
    Q_PROPERTY(QFont TitleMedium MEMBER titleMedium CONSTANT)
    Q_PROPERTY(qreal TitleMediumLineHeight MEMBER titleMediumLineHeight CONSTANT)
    Q_PROPERTY(QFont TitleSmall MEMBER titleSmall CONSTANT)
    Q_PROPERTY(qreal TitleSmallLineHeight MEMBER titleSmallLineHeight CONSTANT)
    Q_PROPERTY(QFont BodyLarge MEMBER bodyLarge CONSTANT)
    Q_PROPERTY(qreal BodyLargeLineHeight MEMBER bodyLargeLineHeight CONSTANT)
    Q_PROPERTY(QFont BodyMedium MEMBER bodyMedium CONSTANT)
    Q_PROPERTY(qreal BodyMediumLineHeight MEMBER bodyMediumLineHeight CONSTANT)
    Q_PROPERTY(QFont BodySmall MEMBER bodySmall CONSTANT)
    Q_PROPERTY(qreal BodySmallLineHeight MEMBER bodySmallLineHeight CONSTANT)
    Q_PROPERTY(QFont LabelLarge MEMBER labelLarge CONSTANT)
    Q_PROPERTY(qreal LabelLargeLineHeight MEMBER labelLargeLineHeight CONSTANT)
    Q_PROPERTY(QFont LabelMedium MEMBER labelMedium CONSTANT)
    Q_PROPERTY(qreal LabelMediumLineHeight MEMBER labelMediumLineHeight CONSTANT)
    Q_PROPERTY(QFont LabelSmall MEMBER labelSmall CONSTANT)
    Q_PROPERTY(qreal LabelSmallLineHeight MEMBER labelSmallLineHeight CONSTANT)

    Q_PROPERTY(QFont DisplayLargeEmphasized MEMBER displayLargeEmphasized CONSTANT)
    Q_PROPERTY(qreal DisplayLargeEmphasizedLineHeight MEMBER displayLargeEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont DisplayMediumEmphasized MEMBER displayMediumEmphasized CONSTANT)
    Q_PROPERTY(qreal DisplayMediumEmphasizedLineHeight MEMBER displayMediumEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont DisplaySmallEmphasized MEMBER displaySmallEmphasized CONSTANT)
    Q_PROPERTY(qreal DisplaySmallEmphasizedLineHeight MEMBER displaySmallEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont HeadlineLargeEmphasized MEMBER headlineLargeEmphasized CONSTANT)
    Q_PROPERTY(qreal HeadlineLargeEmphasizedLineHeight MEMBER headlineLargeEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont HeadlineMediumEmphasized MEMBER headlineMediumEmphasized CONSTANT)
    Q_PROPERTY(qreal HeadlineMediumEmphasizedLineHeight MEMBER headlineMediumEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont HeadlineSmallEmphasized MEMBER headlineSmallEmphasized CONSTANT)
    Q_PROPERTY(qreal HeadlineSmallEmphasizedLineHeight MEMBER headlineSmallEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont TitleLargeEmphasized MEMBER titleLargeEmphasized CONSTANT)
    Q_PROPERTY(qreal TitleLargeEmphasizedLineHeight MEMBER titleLargeEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont TitleMediumEmphasized MEMBER titleMediumEmphasized CONSTANT)
    Q_PROPERTY(qreal TitleMediumEmphasizedLineHeight MEMBER titleMediumEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont TitleSmallEmphasized MEMBER titleSmallEmphasized CONSTANT)
    Q_PROPERTY(qreal TitleSmallEmphasizedLineHeight MEMBER titleSmallEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont BodyLargeEmphasized MEMBER bodyLargeEmphasized CONSTANT)
    Q_PROPERTY(qreal BodyLargeEmphasizedLineHeight MEMBER bodyLargeEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont BodyMediumEmphasized MEMBER bodyMediumEmphasized CONSTANT)
    Q_PROPERTY(qreal BodyMediumEmphasizedLineHeight MEMBER bodyMediumEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont BodySmallEmphasized MEMBER bodySmallEmphasized CONSTANT)
    Q_PROPERTY(qreal BodySmallEmphasizedLineHeight MEMBER bodySmallEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont LabelLargeEmphasized MEMBER labelLargeEmphasized CONSTANT)
    Q_PROPERTY(qreal LabelLargeEmphasizedLineHeight MEMBER labelLargeEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont LabelMediumEmphasized MEMBER labelMediumEmphasized CONSTANT)
    Q_PROPERTY(qreal LabelMediumEmphasizedLineHeight MEMBER labelMediumEmphasizedLineHeight CONSTANT)
    Q_PROPERTY(QFont LabelSmallEmphasized MEMBER labelSmallEmphasized CONSTANT)
    Q_PROPERTY(qreal LabelSmallEmphasizedLineHeight MEMBER labelSmallEmphasizedLineHeight CONSTANT)

public:
    QFont displayLarge;
    qreal displayLargeLineHeight = 64.0;
    QFont displayMedium;
    qreal displayMediumLineHeight = 52.0;
    QFont displaySmall;
    qreal displaySmallLineHeight = 44.0;
    QFont headlineLarge;
    qreal headlineLargeLineHeight = 40.0;
    QFont headlineMedium;
    qreal headlineMediumLineHeight = 36.0;
    QFont headlineSmall;
    qreal headlineSmallLineHeight = 32.0;
    QFont titleLarge;
    qreal titleLargeLineHeight = 28.0;
    QFont titleMedium;
    qreal titleMediumLineHeight = 24.0;
    QFont titleSmall;
    qreal titleSmallLineHeight = 20.0;
    QFont bodyLarge;
    qreal bodyLargeLineHeight = 24.0;
    QFont bodyMedium;
    qreal bodyMediumLineHeight = 20.0;
    QFont bodySmall;
    qreal bodySmallLineHeight = 16.0;
    QFont labelLarge;
    qreal labelLargeLineHeight = 20.0;
    QFont labelMedium;
    qreal labelMediumLineHeight = 16.0;
    QFont labelSmall;
    qreal labelSmallLineHeight = 16.0;

    QFont displayLargeEmphasized;
    qreal displayLargeEmphasizedLineHeight = 64.0;
    QFont displayMediumEmphasized;
    qreal displayMediumEmphasizedLineHeight = 52.0;
    QFont displaySmallEmphasized;
    qreal displaySmallEmphasizedLineHeight = 44.0;
    QFont headlineLargeEmphasized;
    qreal headlineLargeEmphasizedLineHeight = 40.0;
    QFont headlineMediumEmphasized;
    qreal headlineMediumEmphasizedLineHeight = 36.0;
    QFont headlineSmallEmphasized;
    qreal headlineSmallEmphasizedLineHeight = 32.0;
    QFont titleLargeEmphasized;
    qreal titleLargeEmphasizedLineHeight = 28.0;
    QFont titleMediumEmphasized;
    qreal titleMediumEmphasizedLineHeight = 24.0;
    QFont titleSmallEmphasized;
    qreal titleSmallEmphasizedLineHeight = 20.0;
    QFont bodyLargeEmphasized;
    qreal bodyLargeEmphasizedLineHeight = 24.0;
    QFont bodyMediumEmphasized;
    qreal bodyMediumEmphasizedLineHeight = 20.0;
    QFont bodySmallEmphasized;
    qreal bodySmallEmphasizedLineHeight = 16.0;
    QFont labelLargeEmphasized;
    qreal labelLargeEmphasizedLineHeight = 20.0;
    QFont labelMediumEmphasized;
    qreal labelMediumEmphasizedLineHeight = 16.0;
    QFont labelSmallEmphasized;
    qreal labelSmallEmphasizedLineHeight = 16.0;
};
