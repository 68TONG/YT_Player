#include "YT_Theme.h"

#include <QQmlEngine>
#include <QJSEngine>
#include <QColor>
#include <QImage>
#include <QFile>
#include <QTime>
#include <QFontDatabase>

#include "cpp/quantize/celebi.h"
#include "cpp/score/score.h"
#include "cpp/cam/hct.h"
#include "cpp/scheme/scheme_content.h"
#include "cpp/scheme/scheme_expressive.h"
#include "cpp/scheme/scheme_fidelity.h"
#include "cpp/scheme/scheme_fruit_salad.h"
#include "cpp/scheme/scheme_monochrome.h"
#include "cpp/scheme/scheme_neutral.h"
#include "cpp/scheme/scheme_rainbow.h"
#include "cpp/scheme/scheme_tonal_spot.h"
#include "cpp/scheme/scheme_vibrant.h"

YT_Theme::YT_Theme(QObject* parent)
    : QObject{ parent }
{
    themeSource = "#6750A4";
    initScheme();

    {
        int fontId = QFontDatabase::addApplicationFont(":/Resources/Fonts/MaterialSymbols.ttf");
        if (fontId >= 0) {
            QFontDatabase::applicationFontFamilies(fontId);
        }
        fontId = QFontDatabase::addApplicationFont(":/Resources/Fonts/Roboto-VariableFont.ttf");
        if (fontId >= 0) {
            QFontDatabase::applicationFontFamilies(fontId);
        }

        initFont(
            fontBodySmall,
            QStringLiteral("Roboto"),
            12.0,
            400,
            0.1,
            400.0,
            0.0,
            100.0,
            0.0,
            12.0,
            0.0,
            0.0,
            0.0,
            0.0
        );
        fontBodySmallLineHeight = 16.0;

        initFont(
            fontBodyMedium,
            QStringLiteral("Roboto"),
            14.0,
            400,
            0.0,
            400.0,
            0.0,
            100.0,
            0.0,
            14.0,
            0.0,
            0.0,
            0.0,
            0.0
        );
        fontBodyMediumLineHeight = 20.0;

        initFont(
            fontBodyLarge,
            QStringLiteral("Roboto"),
            16.0,
            400,
            0.0,
            400.0,
            0.0,
            100.0,
            0.0,
            16.0,
            0.0,
            0.0,
            0.0,
            0.0
        );
        fontBodyLargeLineHeight = 24.0;
    }

    fontXsmall = QFont("Roboto", 11, 500, false);
    fontSmall = QFont("Roboto", 11, 500, false);
    fontMedium = QFont("Roboto", 12, 500, false);
    fontLarge = QFont("Roboto", 14, 400, false);
    fontXlarge = QFont("Roboto", 14, 400, false);

    connect(this, &YT_Theme::breakPointValueChanged, this, [this]() {
        auto type = static_cast<uint16_t>(BreakPointType::BreakPointExtraLarge);
        for (auto it : breakPointList) {
            if (breakPointValue < it) {
                type = it;
                break;
            }
        }

        if (type == breakPointType) return;
        breakPointType = type;
        emit breakPointTypeChanged();
        });
}

YT_Theme::~YT_Theme()
{
    if (scheme) delete scheme;
}

YT_Theme& YT_Theme::obj()
{
    static YT_Theme object;
    return object;
}

YT_Theme* YT_Theme::create(QQmlEngine*, QJSEngine*)
{
    QJSEngine::setObjectOwnership(&obj(), QJSEngine::CppOwnership);
    return &obj();
}

Q_INVOKABLE QColor YT_Theme::setAlphaF(const QColor& color, float alpha)
{
    QColor newColor = color;
    newColor.setAlphaF(alpha);
    return newColor;
}

YT_Theme::ThemeType YT_Theme::getThemeType() const
{
    return themeType;
}

void YT_Theme::setThemeType(ThemeType data)
{
    if (themeType == data)
        return;

    themeType = data;
    emit themeTypeChanged();
    initScheme();
    emitScheme();
}

QString YT_Theme::getThemeSource() const
{
    return themeSource;
}

void YT_Theme::setThemeSource(QString data)
{
    if (themeSource == data)
        return;

    themeSource = data;
    emit themeSourceChanged();
    initScheme();
    emitScheme();
}

bool YT_Theme::getThemeIsDark() const
{
    return themeIsDark;
}

void YT_Theme::setThemeIsDark(bool data)
{
    if (themeIsDark == data)
        return;

    themeIsDark = data;
    emit themeIsDarkChanged();
    initScheme();
    emitScheme();
}

double YT_Theme::getThemeContrastLevel() const
{
    return themeContrastLevel;
}

void YT_Theme::setThemeContrastLevel(double data)
{
    if (qFuzzyCompare(themeContrastLevel, data))
        return;

    themeContrastLevel = data;
    emit themeContrastLevelChanged();
    initScheme();
    emitScheme();
}

QColor YT_Theme::getTransparent() const
{
    return QColor(0, 0, 0, 0);
}

QColor YT_Theme::getSourceColorArgb() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->SourceColorArgb()));
}

QColor YT_Theme::getPrimaryPaletteKeyColor() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetPrimaryPaletteKeyColor()));
}

QColor YT_Theme::getSecondaryPaletteKeyColor() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSecondaryPaletteKeyColor()));
}

QColor YT_Theme::getTertiaryPaletteKeyColor() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetTertiaryPaletteKeyColor()));
}

QColor YT_Theme::getNeutralPaletteKeyColor() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetNeutralPaletteKeyColor()));
}

QColor YT_Theme::getNeutralVariantPaletteKeyColor() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetNeutralVariantPaletteKeyColor()));
}

QColor YT_Theme::getPrimary() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetPrimary()));
}

QColor YT_Theme::getOnPrimary() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnPrimary()));
}

QColor YT_Theme::getPrimaryContainer() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetPrimaryContainer()));
}

QColor YT_Theme::getOnPrimaryContainer() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnPrimaryContainer()));
}

QColor YT_Theme::getInversePrimary() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetInversePrimary()));
}

QColor YT_Theme::getPrimaryFixed() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetPrimaryFixed()));
}

QColor YT_Theme::getPrimaryFixedDim() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetPrimaryFixedDim()));
}

QColor YT_Theme::getOnPrimaryFixed() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnPrimaryFixed()));
}

QColor YT_Theme::getOnPrimaryFixedVariant() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnPrimaryFixedVariant()));
}

QColor YT_Theme::getSecondary() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSecondary()));
}

QColor YT_Theme::getOnSecondary() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnSecondary()));
}

QColor YT_Theme::getSecondaryContainer() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSecondaryContainer()));
}

QColor YT_Theme::getOnSecondaryContainer() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnSecondaryContainer()));
}

QColor YT_Theme::getSecondaryFixed() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSecondaryFixed()));
}

QColor YT_Theme::getSecondaryFixedDim() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSecondaryFixedDim()));
}

QColor YT_Theme::getOnSecondaryFixed() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnSecondaryFixed()));
}

QColor YT_Theme::getOnSecondaryFixedVariant() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnSecondaryFixedVariant()));
}

QColor YT_Theme::getTertiary() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetTertiary()));
}

QColor YT_Theme::getOnTertiary() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnTertiary()));
}

QColor YT_Theme::getTertiaryContainer() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetTertiaryContainer()));
}

QColor YT_Theme::getOnTertiaryContainer() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnTertiaryContainer()));
}

QColor YT_Theme::getTertiaryFixed() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetTertiaryFixed()));
}

QColor YT_Theme::getTertiaryFixedDim() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetTertiaryFixedDim()));
}

QColor YT_Theme::getOnTertiaryFixed() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnTertiaryFixed()));
}

QColor YT_Theme::getOnTertiaryFixedVariant() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnTertiaryFixedVariant()));
}

QColor YT_Theme::getBackground() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetBackground()));
}

QColor YT_Theme::getOnBackground() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnBackground()));
}

QColor YT_Theme::getSurface() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSurface()));
}

QColor YT_Theme::getSurfaceDim() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSurfaceDim()));
}

QColor YT_Theme::getSurfaceBright() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSurfaceBright()));
}

QColor YT_Theme::getSurfaceContainerLowest() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSurfaceContainerLowest()));
}

QColor YT_Theme::getSurfaceContainerLow() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSurfaceContainerLow()));
}

QColor YT_Theme::getSurfaceContainer() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSurfaceContainer()));
}

QColor YT_Theme::getSurfaceContainerHigh() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSurfaceContainerHigh()));
}

QColor YT_Theme::getSurfaceContainerHighest() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSurfaceContainerHighest()));
}

QColor YT_Theme::getOnSurface() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnSurface()));
}

QColor YT_Theme::getSurfaceVariant() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSurfaceVariant()));
}

QColor YT_Theme::getOnSurfaceVariant() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnSurfaceVariant()));
}

QColor YT_Theme::getInverseSurface() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetInverseSurface()));
}

QColor YT_Theme::getInverseOnSurface() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetInverseOnSurface()));
}

QColor YT_Theme::getOutline() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOutline()));
}

QColor YT_Theme::getOutlineVariant() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOutlineVariant()));
}

QColor YT_Theme::getShadow() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetShadow()));
}

QColor YT_Theme::getScrim() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetScrim()));
}

QColor YT_Theme::getSurfaceTint() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetSurfaceTint()));
}

QColor YT_Theme::getError() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetError()));
}

QColor YT_Theme::getOnError() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnError()));
}

QColor YT_Theme::getErrorContainer() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetErrorContainer()));
}

QColor YT_Theme::getOnErrorContainer() const
{
    return QColor::fromRgba(static_cast<QRgb>(scheme->GetOnErrorContainer()));
}

int YT_Theme::getElevationNone() const
{
    return INT_MIN;
}

int YT_Theme::getElevation0() const
{
    return 0;
}

int YT_Theme::getElevation1() const
{
    return 1;
}

int YT_Theme::getElevation2() const
{
    return 3;
}

int YT_Theme::getElevation3() const
{
    return 6;
}

int YT_Theme::getElevation4() const
{
    return 8;
}

int YT_Theme::getElevation5() const
{
    return 12;
}

int YT_Theme::getRadiusNone() const
{
    return 0;
}

int YT_Theme::getRadiusExtraSmall() const
{
    return 4;
}

int YT_Theme::getRadiusSmall() const
{
    return 8;
}

int YT_Theme::getRadiusMedium() const
{
    return 12;
}

int YT_Theme::getRadiusLarge() const
{
    return 16;
}

int YT_Theme::getRadiusLargeIncreased() const
{
    return 20;
}

int YT_Theme::getRadiusExtraLarge() const
{
    return 28;
}

int YT_Theme::getRadiusExtraLargeIncreased() const
{
    return 32;
}

int YT_Theme::getRadiusExtraExtraLarge() const
{
    return 48;
}

int YT_Theme::getRadiusFull() const
{
    return INT_MAX;
}

int YT_Theme::getSpace0() const
{
    return 0;
}

int YT_Theme::getSpace25() const
{
    return 2;
}

int YT_Theme::getSpace50() const
{
    return 4;
}

int YT_Theme::getSpace75() const
{
    return 6;
}

int YT_Theme::getSpace100() const
{
    return 8;
}

int YT_Theme::getSpace125() const
{
    return 10;
}

int YT_Theme::getSpace150() const
{
    return 12;
}

int YT_Theme::getSpace175() const
{
    return 14;
}

int YT_Theme::getSpace200() const
{
    return 16;
}

int YT_Theme::getSpace250() const
{
    return 20;
}

int YT_Theme::getSpace300() const
{
    return 24;
}

int YT_Theme::getSpace400() const
{
    return 32;
}

int YT_Theme::getSpace450() const
{
    return 36;
}

int YT_Theme::getSpace500() const
{
    return 40;
}

int YT_Theme::getSpace600() const
{
    return 48;
}

int YT_Theme::getSpace700() const
{
    return 56;
}

int YT_Theme::getSpace800() const
{
    return 64;
}

int YT_Theme::getSpace900() const
{
    return 72;
}


QFont YT_Theme::getFontBodySmall() const
{
    return fontBodySmall;
}

double YT_Theme::getFontBodySmallLineHeight() const
{
    return fontBodySmallLineHeight;
}

QFont YT_Theme::getFontBodyMedium() const
{
    return fontBodyMedium;
}

double YT_Theme::getFontBodyMediumLineHeight() const
{
    return fontBodyMediumLineHeight;
}

QFont YT_Theme::getFontBodyLarge() const
{
    return fontBodyLarge;
}

double YT_Theme::getFontBodyLargeLineHeight() const
{
    return fontBodyLargeLineHeight;
}

QFont YT_Theme::getFontXsmall() const
{
    return fontXsmall;
}

QFont YT_Theme::getFontSmall() const
{
    return fontSmall;
}

QFont YT_Theme::getFontMedium() const
{
    return fontMedium;
}

QFont YT_Theme::getFontLarge() const
{
    return fontLarge;
}

QFont YT_Theme::getFontXlarge() const
{
    return fontXlarge;
}

void YT_Theme::initScheme()
{
    if (scheme) {
        delete scheme;
        scheme = nullptr;
    }

    auto createScheme = [&](const material_color_utilities::Hct& sourceColor) {
        switch (themeType) {
        case ThemeType::SchemeContent:
            scheme = new material_color_utilities::SchemeContent(sourceColor, themeIsDark, themeContrastLevel);
            break;
        case ThemeType::SchemeVibrant:
            scheme = new material_color_utilities::SchemeVibrant(sourceColor, themeIsDark, themeContrastLevel);
            break;
        case ThemeType::SchemeExpressive:
            scheme = new material_color_utilities::SchemeExpressive(sourceColor, themeIsDark, themeContrastLevel);
            break;
        case ThemeType::SchemeFidelity:
            scheme = new material_color_utilities::SchemeFidelity(sourceColor, themeIsDark, themeContrastLevel);
            break;
        case ThemeType::SchemeNeutral:
            scheme = new material_color_utilities::SchemeNeutral(sourceColor, themeIsDark, themeContrastLevel);
            break;
        case ThemeType::SchemeMonochrome:
            scheme = new material_color_utilities::SchemeMonochrome(sourceColor, themeIsDark, themeContrastLevel);
            break;
        case ThemeType::SchemeTonalSpot:
            scheme = new material_color_utilities::SchemeTonalSpot(sourceColor, themeIsDark, themeContrastLevel);
            break;
        case ThemeType::SchemeRainbow:
            scheme = new material_color_utilities::SchemeRainbow(sourceColor, themeIsDark, themeContrastLevel);
            break;
        case ThemeType::SchemeFruitSalad:
            scheme = new material_color_utilities::SchemeFruitSalad(sourceColor, themeIsDark, themeContrastLevel);
            break;
        default:
            scheme = new material_color_utilities::SchemeContent(sourceColor, themeIsDark, themeContrastLevel);
            break;
        }
        };

    if (QFile::exists(QUrl(themeSource).toLocalFile()) == false) {
        const auto sourceArgb = static_cast<material_color_utilities::Argb>(QColor(themeSource).rgba());
        material_color_utilities::Hct sourceColor{ sourceArgb };
        createScheme(sourceColor);
        return;
    }

    QImage image(QUrl(themeSource).toLocalFile());
    if (image.isNull()) {
        qWarning("YT_Theme::setThemeSource image.isNull() themeSource = %s", qUtf8Printable(themeSource));
        return;
    }
    image = image.scaledToWidth(512, Qt::SmoothTransformation);

    std::vector<material_color_utilities::Argb> pixels;
    pixels.reserve(image.width() * image.height());

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb rgb = image.pixel(x, y);
            pixels.push_back(static_cast<material_color_utilities::Argb>(rgb));
        }
    }

    material_color_utilities::QuantizerResult colorToCount =
        material_color_utilities::QuantizeCelebi(pixels, 128);

    material_color_utilities::ScoreOptions options;
    options.desired = 4;
    options.fallback_color_argb = 0xff4b1ec5;
    options.filter = true;

    std::vector<material_color_utilities::Argb> rankedColors =
        material_color_utilities::RankedSuggestions(colorToCount.color_to_count, options);

    material_color_utilities::Hct sourceColor{ rankedColors[0] };
    createScheme(sourceColor);
}

void YT_Theme::emitScheme()
{
    emit sourceColorArgbChanged();
    emit primaryPaletteKeyColorChanged();
    emit secondaryPaletteKeyColorChanged();
    emit tertiaryPaletteKeyColorChanged();
    emit neutralPaletteKeyColorChanged();
    emit neutralVariantPaletteKeyColorChanged();
    emit primaryChanged();
    emit onPrimaryChanged();
    emit primaryContainerChanged();
    emit onPrimaryContainerChanged();
    emit inversePrimaryChanged();
    emit primaryFixedChanged();
    emit primaryFixedDimChanged();
    emit onPrimaryFixedChanged();
    emit onPrimaryFixedVariantChanged();
    emit secondaryChanged();
    emit onSecondaryChanged();
    emit secondaryContainerChanged();
    emit onSecondaryContainerChanged();
    emit secondaryFixedChanged();
    emit secondaryFixedDimChanged();
    emit onSecondaryFixedChanged();
    emit onSecondaryFixedVariantChanged();
    emit tertiaryChanged();
    emit onTertiaryChanged();
    emit tertiaryContainerChanged();
    emit onTertiaryContainerChanged();
    emit tertiaryFixedChanged();
    emit tertiaryFixedDimChanged();
    emit onTertiaryFixedChanged();
    emit onTertiaryFixedVariantChanged();
    emit backgroundChanged();
    emit onBackgroundChanged();
    emit surfaceChanged();
    emit surfaceDimChanged();
    emit surfaceBrightChanged();
    emit surfaceContainerLowestChanged();
    emit surfaceContainerLowChanged();
    emit surfaceContainerChanged();
    emit surfaceContainerHighChanged();
    emit surfaceContainerHighestChanged();
    emit onSurfaceChanged();
    emit surfaceVariantChanged();
    emit onSurfaceVariantChanged();
    emit inverseSurfaceChanged();
    emit inverseOnSurfaceChanged();
    emit outlineChanged();
    emit outlineVariantChanged();
    emit shadowChanged();
    emit scrimChanged();
    emit surfaceTintChanged();
    emit errorChanged();
    emit onErrorChanged();
    emit errorContainerChanged();
    emit onErrorContainerChanged();

    // elevation is not changed, so no need to emit signals for elevation properties
    // emit elevation0Changed();
    // emit elevation1Changed();
    // emit elevation2Changed();
    // emit elevation3Changed();
    // emit elevation4Changed();
    // emit elevation5Changed();
}

uint16_t YT_Theme::getBreakPointType() const
{
    return breakPointType;
}

void YT_Theme::initFont(QFont& font, const QString& family, double pointSize, int weight, double letterSpacing, double wght, double grad, double wdth, double rond, double opsz, double crsv, double slnt, double fill, double hexp)
{
    font.setFamily(family);
    font.setPixelSize(pointSize);
    font.setWeight(static_cast<QFont::Weight>(weight));
    font.setLetterSpacing(QFont::AbsoluteSpacing, letterSpacing);

    font.setVariableAxis(QFont::Tag("wght"), wght);
    font.setVariableAxis(QFont::Tag("GRAD"), grad);
    font.setVariableAxis(QFont::Tag("wdth"), wdth);
    font.setVariableAxis(QFont::Tag("ROND"), rond);
    font.setVariableAxis(QFont::Tag("opsz"), opsz);
    font.setVariableAxis(QFont::Tag("CRSV"), crsv);
    font.setVariableAxis(QFont::Tag("slnt"), slnt);
    font.setVariableAxis(QFont::Tag("FILL"), fill);
    font.setVariableAxis(QFont::Tag("HEXP"), hexp);
}
