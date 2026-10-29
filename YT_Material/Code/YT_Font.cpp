#include "YT_Font.h"

#include <QFontDatabase>
#include <QFontInfo>
#include <QJSEngine>
#include <QQmlEngine>
#include <QVariantMap>

YT_Font::YT_Font(QObject* parent)
    : QObject{ parent }
{
    if (QFontDatabase::addApplicationFont(":/Resources/Fonts/MaterialSymbols.ttf") < 0) {
        qWarning("YT_Font: failed to load Material Symbols font");
    }
    if (QFontDatabase::addApplicationFont(":/Resources/Fonts/Roboto-VariableFont.ttf") < 0) {
        qWarning("YT_Font: failed to load Roboto variable font");
    }

    displayLarge = createFont(QStringLiteral("Roboto"), 57, 400, -0.2);
    displayMedium = createFont(QStringLiteral("Roboto"), 45, 400, 0.0);
    displaySmall = createFont(QStringLiteral("Roboto"), 36, 400, 0.0);
    headlineLarge = createFont(QStringLiteral("Roboto"), 32, 400, 0.0);
    headlineMedium = createFont(QStringLiteral("Roboto"), 28, 400, 0.0);
    headlineSmall = createFont(QStringLiteral("Roboto"), 24, 400, 0.0);
    titleLarge = createFont(QStringLiteral("Roboto"), 22, 400, 0.0);
    titleMedium = createFont(QStringLiteral("Roboto"), 16, 500, 0.2);
    titleSmall = createFont(QStringLiteral("Roboto"), 14, 500, 0.1);
    bodyLarge = createFont(QStringLiteral("Roboto"), 16, 400, 0.5);
    bodyMedium = createFont(QStringLiteral("Roboto"), 14, 400, 0.2);
    bodySmall = createFont(QStringLiteral("Roboto"), 12, 400, 0.4);
    labelLarge = createFont(QStringLiteral("Roboto"), 14, 500, 0.1);
    labelMedium = createFont(QStringLiteral("Roboto"), 12, 500, 0.5);
    labelSmall = createFont(QStringLiteral("Roboto"), 11, 500, 0.5);

    displayLargeEmphasized = createFont(QStringLiteral("Roboto"), 57, 500, 0.0);
    displayMediumEmphasized = createFont(QStringLiteral("Roboto"), 45, 500, 0.0);
    displaySmallEmphasized = createFont(QStringLiteral("Roboto"), 36, 500, 0.0);
    headlineLargeEmphasized = createFont(QStringLiteral("Roboto"), 32, 500, 0.0);
    headlineMediumEmphasized = createFont(QStringLiteral("Roboto"), 28, 500, 0.0);
    headlineSmallEmphasized = createFont(QStringLiteral("Roboto"), 24, 500, 0.0);
    titleLargeEmphasized = createFont(QStringLiteral("Roboto"), 22, 500, 0.0);
    titleMediumEmphasized = createFont(QStringLiteral("Roboto"), 16, 700, 0.15);
    titleSmallEmphasized = createFont(QStringLiteral("Roboto"), 14, 700, 0.1);
    bodyLargeEmphasized = createFont(QStringLiteral("Roboto"), 16, 500, 0.15);
    bodyMediumEmphasized = createFont(QStringLiteral("Roboto"), 14, 500, 0.25);
    bodySmallEmphasized = createFont(QStringLiteral("Roboto"), 12, 500, 0.4);
    labelLargeEmphasized = createFont(QStringLiteral("Roboto"), 14, 700, 0.1);
    labelMediumEmphasized = createFont(QStringLiteral("Roboto"), 12, 700, 0.5);
    labelSmallEmphasized = createFont(QStringLiteral("Roboto"), 11, 700, 0.5);
}

YT_Font& YT_Font::obj()
{
    static YT_Font object;
    return object;
}

YT_Font* YT_Font::create(QQmlEngine*, QJSEngine*)
{
    QQmlEngine::setObjectOwnership(&obj(), QQmlEngine::CppOwnership);
    return &obj();
}

QFont YT_Font::createFont(
    const QString& family,
    qreal pixelSize,
    int weight,
    qreal letterSpacing
) const
{
    QFont font;
    font.setFamily(family);
    font.setPixelSize(qRound(pixelSize));
    font.setWeight(static_cast<QFont::Weight>(weight));
    font.setLetterSpacing(QFont::AbsoluteSpacing, letterSpacing);
    font.setVariableAxis(QFont::Tag("wght"), weight);
    font.setVariableAxis(QFont::Tag("wdth"), 100.0);
    return font;
}

QVariantList YT_Font::getVariableAxes(const QFont& font) const
{
    QVariantList axes;
    const QFontInfo fontInfo(font);
    for (const QFontVariableAxis& axis : fontInfo.variableAxes()) {
        const QFont::Tag tag = axis.tag();
        QVariantMap data;
        data.insert(QStringLiteral("tag"), QString::fromLatin1(tag.toString()));
        data.insert(QStringLiteral("name"), axis.name());
        data.insert(QStringLiteral("minimumValue"), axis.minimumValue());
        data.insert(QStringLiteral("defaultValue"), axis.defaultValue());
        data.insert(QStringLiteral("maximumValue"), axis.maximumValue());
        data.insert(QStringLiteral("value"), font.isVariableAxisSet(tag) ? font.variableAxisValue(tag) : axis.defaultValue());
        axes.append(data);
    }
    return axes;
}

QVariantList YT_Font::getRobotoVariableAxes() const
{
    return getVariableAxes(createFont(QStringLiteral("Roboto"), 16, 400, 0.0));
}
