#pragma once

#include "YT_Global.h"
#include "cpp/scheme/scheme_content.h"
#include <QColor>
#include <QQuickItem>

class YT_EXPORT YT_Theme : public QObject
{
    Q_OBJECT
        QML_SINGLETON
        QML_NAMED_ELEMENT(Theme)
private:
    explicit YT_Theme(QObject* parent = nullptr);
    ~YT_Theme();
public:
    static YT_Theme& obj();
    static YT_Theme* create(QQmlEngine*, QJSEngine*);

    Q_INVOKABLE static QColor setAlphaF(const QColor& color, float alpha);

    enum class ThemeType : uint8_t {
        SchemeContent = 0,
        SchemeVibrant,
        SchemeExpressive,
        SchemeFidelity,
        SchemeNeutral,
        SchemeMonochrome,
        SchemeTonalSpot,
        SchemeRainbow,
        SchemeFruitSalad,
    };
    Q_ENUM(ThemeType);

    Q_PROPERTY(ThemeType themeType READ getThemeType WRITE setThemeType NOTIFY themeTypeChanged)
        ThemeType themeType = ThemeType::SchemeContent;
    ThemeType getThemeType() const;
    void setThemeType(ThemeType data);
    Q_SIGNAL void themeTypeChanged();

    Q_PROPERTY(QString themeSource READ getThemeSource WRITE setThemeSource NOTIFY themeSourceChanged)
        QString themeSource;
    QString getThemeSource() const;
    void setThemeSource(QString data);
    Q_SIGNAL void themeSourceChanged();

    Q_PROPERTY(bool themeIsDark READ getThemeIsDark WRITE setThemeIsDark NOTIFY themeIsDarkChanged)
        bool themeIsDark = false;
    bool getThemeIsDark() const;
    void setThemeIsDark(bool data);
    Q_SIGNAL void themeIsDarkChanged();

    Q_PROPERTY(double themeContrastLevel READ getThemeContrastLevel WRITE setThemeContrastLevel NOTIFY themeContrastLevelChanged)
        double themeContrastLevel = 0.0;
    double getThemeContrastLevel() const;
    void setThemeContrastLevel(double data);
    Q_SIGNAL void themeContrastLevelChanged();

    Q_PROPERTY(QColor Transparent READ getTransparent NOTIFY transparentChanged)
        QColor getTransparent() const;
    Q_SIGNAL void transparentChanged();

    Q_PROPERTY(QColor SourceColorArgb READ getSourceColorArgb NOTIFY sourceColorArgbChanged)
        QColor getSourceColorArgb() const;
    Q_SIGNAL void sourceColorArgbChanged();

    Q_PROPERTY(QColor PrimaryPaletteKeyColor READ getPrimaryPaletteKeyColor NOTIFY primaryPaletteKeyColorChanged)
        QColor getPrimaryPaletteKeyColor() const;
    Q_SIGNAL void primaryPaletteKeyColorChanged();

    Q_PROPERTY(QColor SecondaryPaletteKeyColor READ getSecondaryPaletteKeyColor NOTIFY secondaryPaletteKeyColorChanged)
        QColor getSecondaryPaletteKeyColor() const;
    Q_SIGNAL void secondaryPaletteKeyColorChanged();

    Q_PROPERTY(QColor TertiaryPaletteKeyColor READ getTertiaryPaletteKeyColor NOTIFY tertiaryPaletteKeyColorChanged)
        QColor getTertiaryPaletteKeyColor() const;
    Q_SIGNAL void tertiaryPaletteKeyColorChanged();

    Q_PROPERTY(QColor NeutralPaletteKeyColor READ getNeutralPaletteKeyColor NOTIFY neutralPaletteKeyColorChanged)
        QColor getNeutralPaletteKeyColor() const;
    Q_SIGNAL void neutralPaletteKeyColorChanged();

    Q_PROPERTY(QColor NeutralVariantPaletteKeyColor READ getNeutralVariantPaletteKeyColor NOTIFY neutralVariantPaletteKeyColorChanged)
        QColor getNeutralVariantPaletteKeyColor() const;
    Q_SIGNAL void neutralVariantPaletteKeyColorChanged();

    Q_PROPERTY(QColor Primary READ getPrimary NOTIFY primaryChanged)
        QColor getPrimary() const;
    Q_SIGNAL void primaryChanged();

    Q_PROPERTY(QColor OnPrimary READ getOnPrimary NOTIFY onPrimaryChanged)
        QColor getOnPrimary() const;
    Q_SIGNAL void onPrimaryChanged();

    Q_PROPERTY(QColor PrimaryContainer READ getPrimaryContainer NOTIFY primaryContainerChanged)
        QColor getPrimaryContainer() const;
    Q_SIGNAL void primaryContainerChanged();

    Q_PROPERTY(QColor OnPrimaryContainer READ getOnPrimaryContainer NOTIFY onPrimaryContainerChanged)
        QColor getOnPrimaryContainer() const;
    Q_SIGNAL void onPrimaryContainerChanged();

    Q_PROPERTY(QColor InversePrimary READ getInversePrimary NOTIFY inversePrimaryChanged)
        QColor getInversePrimary() const;
    Q_SIGNAL void inversePrimaryChanged();

    Q_PROPERTY(QColor PrimaryFixed READ getPrimaryFixed NOTIFY primaryFixedChanged)
        QColor getPrimaryFixed() const;
    Q_SIGNAL void primaryFixedChanged();

    Q_PROPERTY(QColor PrimaryFixedDim READ getPrimaryFixedDim NOTIFY primaryFixedDimChanged)
        QColor getPrimaryFixedDim() const;
    Q_SIGNAL void primaryFixedDimChanged();

    Q_PROPERTY(QColor OnPrimaryFixed READ getOnPrimaryFixed NOTIFY onPrimaryFixedChanged)
        QColor getOnPrimaryFixed() const;
    Q_SIGNAL void onPrimaryFixedChanged();

    Q_PROPERTY(QColor OnPrimaryFixedVariant READ getOnPrimaryFixedVariant NOTIFY onPrimaryFixedVariantChanged)
        QColor getOnPrimaryFixedVariant() const;
    Q_SIGNAL void onPrimaryFixedVariantChanged();

    Q_PROPERTY(QColor Secondary READ getSecondary NOTIFY secondaryChanged)
        QColor getSecondary() const;
    Q_SIGNAL void secondaryChanged();

    Q_PROPERTY(QColor OnSecondary READ getOnSecondary NOTIFY onSecondaryChanged)
        QColor getOnSecondary() const;
    Q_SIGNAL void onSecondaryChanged();

    Q_PROPERTY(QColor SecondaryContainer READ getSecondaryContainer NOTIFY secondaryContainerChanged)
        QColor getSecondaryContainer() const;
    Q_SIGNAL void secondaryContainerChanged();

    Q_PROPERTY(QColor OnSecondaryContainer READ getOnSecondaryContainer NOTIFY onSecondaryContainerChanged)
        QColor getOnSecondaryContainer() const;
    Q_SIGNAL void onSecondaryContainerChanged();

    Q_PROPERTY(QColor SecondaryFixed READ getSecondaryFixed NOTIFY secondaryFixedChanged)
        QColor getSecondaryFixed() const;
    Q_SIGNAL void secondaryFixedChanged();

    Q_PROPERTY(QColor SecondaryFixedDim READ getSecondaryFixedDim NOTIFY secondaryFixedDimChanged)
        QColor getSecondaryFixedDim() const;
    Q_SIGNAL void secondaryFixedDimChanged();

    Q_PROPERTY(QColor OnSecondaryFixed READ getOnSecondaryFixed NOTIFY onSecondaryFixedChanged)
        QColor getOnSecondaryFixed() const;
    Q_SIGNAL void onSecondaryFixedChanged();

    Q_PROPERTY(QColor OnSecondaryFixedVariant READ getOnSecondaryFixedVariant NOTIFY onSecondaryFixedVariantChanged)
        QColor getOnSecondaryFixedVariant() const;
    Q_SIGNAL void onSecondaryFixedVariantChanged();

    Q_PROPERTY(QColor Tertiary READ getTertiary NOTIFY tertiaryChanged)
        QColor getTertiary() const;
    Q_SIGNAL void tertiaryChanged();

    Q_PROPERTY(QColor OnTertiary READ getOnTertiary NOTIFY onTertiaryChanged)
        QColor getOnTertiary() const;
    Q_SIGNAL void onTertiaryChanged();

    Q_PROPERTY(QColor TertiaryContainer READ getTertiaryContainer NOTIFY tertiaryContainerChanged)
        QColor getTertiaryContainer() const;
    Q_SIGNAL void tertiaryContainerChanged();

    Q_PROPERTY(QColor OnTertiaryContainer READ getOnTertiaryContainer NOTIFY onTertiaryContainerChanged)
        QColor getOnTertiaryContainer() const;
    Q_SIGNAL void onTertiaryContainerChanged();

    Q_PROPERTY(QColor TertiaryFixed READ getTertiaryFixed NOTIFY tertiaryFixedChanged)
        QColor getTertiaryFixed() const;
    Q_SIGNAL void tertiaryFixedChanged();

    Q_PROPERTY(QColor TertiaryFixedDim READ getTertiaryFixedDim NOTIFY tertiaryFixedDimChanged)
        QColor getTertiaryFixedDim() const;
    Q_SIGNAL void tertiaryFixedDimChanged();

    Q_PROPERTY(QColor OnTertiaryFixed READ getOnTertiaryFixed NOTIFY onTertiaryFixedChanged)
        QColor getOnTertiaryFixed() const;
    Q_SIGNAL void onTertiaryFixedChanged();

    Q_PROPERTY(QColor OnTertiaryFixedVariant READ getOnTertiaryFixedVariant NOTIFY onTertiaryFixedVariantChanged)
        QColor getOnTertiaryFixedVariant() const;
    Q_SIGNAL void onTertiaryFixedVariantChanged();

    Q_PROPERTY(QColor Background READ getBackground NOTIFY backgroundChanged)
        QColor getBackground() const;
    Q_SIGNAL void backgroundChanged();

    Q_PROPERTY(QColor OnBackground READ getOnBackground NOTIFY onBackgroundChanged)
        QColor getOnBackground() const;
    Q_SIGNAL void onBackgroundChanged();

    Q_PROPERTY(QColor Surface READ getSurface NOTIFY surfaceChanged)
        QColor getSurface() const;
    Q_SIGNAL void surfaceChanged();

    Q_PROPERTY(QColor SurfaceDim READ getSurfaceDim NOTIFY surfaceDimChanged)
        QColor getSurfaceDim() const;
    Q_SIGNAL void surfaceDimChanged();

    Q_PROPERTY(QColor SurfaceBright READ getSurfaceBright NOTIFY surfaceBrightChanged)
        QColor getSurfaceBright() const;
    Q_SIGNAL void surfaceBrightChanged();

    Q_PROPERTY(QColor SurfaceContainerLowest READ getSurfaceContainerLowest NOTIFY surfaceContainerLowestChanged)
        QColor getSurfaceContainerLowest() const;
    Q_SIGNAL void surfaceContainerLowestChanged();

    Q_PROPERTY(QColor SurfaceContainerLow READ getSurfaceContainerLow NOTIFY surfaceContainerLowChanged)
        QColor getSurfaceContainerLow() const;
    Q_SIGNAL void surfaceContainerLowChanged();

    Q_PROPERTY(QColor SurfaceContainer READ getSurfaceContainer NOTIFY surfaceContainerChanged)
        QColor getSurfaceContainer() const;
    Q_SIGNAL void surfaceContainerChanged();

    Q_PROPERTY(QColor SurfaceContainerHigh READ getSurfaceContainerHigh NOTIFY surfaceContainerHighChanged)
        QColor getSurfaceContainerHigh() const;
    Q_SIGNAL void surfaceContainerHighChanged();

    Q_PROPERTY(QColor SurfaceContainerHighest READ getSurfaceContainerHighest NOTIFY surfaceContainerHighestChanged)
        QColor getSurfaceContainerHighest() const;
    Q_SIGNAL void surfaceContainerHighestChanged();

    Q_PROPERTY(QColor OnSurface READ getOnSurface NOTIFY onSurfaceChanged)
        QColor getOnSurface() const;
    Q_SIGNAL void onSurfaceChanged();

    Q_PROPERTY(QColor SurfaceVariant READ getSurfaceVariant NOTIFY surfaceVariantChanged)
        QColor getSurfaceVariant() const;
    Q_SIGNAL void surfaceVariantChanged();

    Q_PROPERTY(QColor OnSurfaceVariant READ getOnSurfaceVariant NOTIFY onSurfaceVariantChanged)
        QColor getOnSurfaceVariant() const;
    Q_SIGNAL void onSurfaceVariantChanged();

    Q_PROPERTY(QColor InverseSurface READ getInverseSurface NOTIFY inverseSurfaceChanged)
        QColor getInverseSurface() const;
    Q_SIGNAL void inverseSurfaceChanged();

    Q_PROPERTY(QColor InverseOnSurface READ getInverseOnSurface NOTIFY inverseOnSurfaceChanged)
        QColor getInverseOnSurface() const;
    Q_SIGNAL void inverseOnSurfaceChanged();

    Q_PROPERTY(QColor Outline READ getOutline NOTIFY outlineChanged)
        QColor getOutline() const;
    Q_SIGNAL void outlineChanged();

    Q_PROPERTY(QColor OutlineVariant READ getOutlineVariant NOTIFY outlineVariantChanged)
        QColor getOutlineVariant() const;
    Q_SIGNAL void outlineVariantChanged();

    Q_PROPERTY(QColor Shadow READ getShadow NOTIFY shadowChanged)
        QColor getShadow() const;
    Q_SIGNAL void shadowChanged();

    Q_PROPERTY(QColor Scrim READ getScrim NOTIFY scrimChanged)
        QColor getScrim() const;
    Q_SIGNAL void scrimChanged();

    Q_PROPERTY(QColor SurfaceTint READ getSurfaceTint NOTIFY surfaceTintChanged)
        QColor getSurfaceTint() const;
    Q_SIGNAL void surfaceTintChanged();

    Q_PROPERTY(QColor Error READ getError NOTIFY errorChanged)
        QColor getError() const;
    Q_SIGNAL void errorChanged();

    Q_PROPERTY(QColor OnError READ getOnError NOTIFY onErrorChanged)
        QColor getOnError() const;
    Q_SIGNAL void onErrorChanged();

    Q_PROPERTY(QColor ErrorContainer READ getErrorContainer NOTIFY errorContainerChanged)
        QColor getErrorContainer() const;
    Q_SIGNAL void errorContainerChanged();

    Q_PROPERTY(QColor OnErrorContainer READ getOnErrorContainer NOTIFY onErrorContainerChanged)
        QColor getOnErrorContainer() const;
    Q_SIGNAL void onErrorContainerChanged();

    Q_PROPERTY(int ElevationNone READ getElevationNone CONSTANT)
        int getElevationNone() const;

    Q_PROPERTY(int Elevation0 READ getElevation0 CONSTANT)
        int getElevation0() const;

    Q_PROPERTY(int Elevation1 READ getElevation1 CONSTANT)
        int getElevation1() const;

    Q_PROPERTY(int Elevation2 READ getElevation2 CONSTANT)
        int getElevation2() const;

    Q_PROPERTY(int Elevation3 READ getElevation3 CONSTANT)
        int getElevation3() const;

    Q_PROPERTY(int Elevation4 READ getElevation4 CONSTANT)
        int getElevation4() const;

    Q_PROPERTY(int Elevation5 READ getElevation5 CONSTANT)
        int getElevation5() const;

    Q_PROPERTY(int RadiusNone READ getRadiusNone NOTIFY radiusNoneChanged)
        int getRadiusNone() const;
    Q_SIGNAL void radiusNoneChanged();

    Q_PROPERTY(int RadiusExtraSmall READ getRadiusExtraSmall NOTIFY radiusExtraSmallChanged)
        int getRadiusExtraSmall() const;
    Q_SIGNAL void radiusExtraSmallChanged();

    Q_PROPERTY(int RadiusSmall READ getRadiusSmall NOTIFY radiusSmallChanged)
        int getRadiusSmall() const;
    Q_SIGNAL void radiusSmallChanged();

    Q_PROPERTY(int RadiusMedium READ getRadiusMedium NOTIFY radiusMediumChanged)
        int getRadiusMedium() const;
    Q_SIGNAL void radiusMediumChanged();

    Q_PROPERTY(int RadiusLarge READ getRadiusLarge NOTIFY radiusLargeChanged)
        int getRadiusLarge() const;
    Q_SIGNAL void radiusLargeChanged();

    Q_PROPERTY(int RadiusLargeIncreased READ getRadiusLargeIncreased NOTIFY radiusLargeIncreasedChanged)
        int getRadiusLargeIncreased() const;
    Q_SIGNAL void radiusLargeIncreasedChanged();

    Q_PROPERTY(int RadiusExtraLarge READ getRadiusExtraLarge NOTIFY radiusExtraLargeChanged)
        int getRadiusExtraLarge() const;
    Q_SIGNAL void radiusExtraLargeChanged();

    Q_PROPERTY(int RadiusExtraLargeIncreased READ getRadiusExtraLargeIncreased NOTIFY radiusExtraLargeIncreasedChanged)
        int getRadiusExtraLargeIncreased() const;
    Q_SIGNAL void radiusExtraLargeIncreasedChanged();

    Q_PROPERTY(int RadiusExtraExtraLarge READ getRadiusExtraExtraLarge NOTIFY radiusExtraExtraLargeChanged)
        int getRadiusExtraExtraLarge() const;
    Q_SIGNAL void radiusExtraExtraLargeChanged();

    Q_PROPERTY(int RadiusFull READ getRadiusFull NOTIFY radiusFullChanged)
        int getRadiusFull() const;
    Q_SIGNAL void radiusFullChanged();

    Q_PROPERTY(int Space0 READ getSpace0 NOTIFY space0Changed)
        int getSpace0() const;
    Q_SIGNAL void space0Changed();

    Q_PROPERTY(int Space25 READ getSpace25 NOTIFY space25Changed)
        int getSpace25() const;
    Q_SIGNAL void space25Changed();

    Q_PROPERTY(int Space50 READ getSpace50 NOTIFY space50Changed)
        int getSpace50() const;
    Q_SIGNAL void space50Changed();

    /// @property int Theme::Space75 @brief Space75 is 6
    /// @property int YT_Theme::Space75 @brief Space75 is 66
    Q_PROPERTY(int Space75 READ getSpace75 NOTIFY space75Changed)
        int getSpace75() const;
    Q_SIGNAL void space75Changed();

    Q_PROPERTY(int Space100 READ getSpace100 NOTIFY space100Changed)
        int getSpace100() const;
    Q_SIGNAL void space100Changed();

    Q_PROPERTY(int Space125 READ getSpace125 NOTIFY space125Changed)
        int getSpace125() const;
    Q_SIGNAL void space125Changed();

    Q_PROPERTY(int Space150 READ getSpace150 NOTIFY space150Changed)
        int getSpace150() const;
    Q_SIGNAL void space150Changed();

    Q_PROPERTY(int Space175 READ getSpace175 NOTIFY space175Changed)
        int getSpace175() const;
    Q_SIGNAL void space175Changed();

    Q_PROPERTY(int Space200 READ getSpace200 NOTIFY space200Changed)
        int getSpace200() const;
    Q_SIGNAL void space200Changed();

    Q_PROPERTY(int Space250 READ getSpace250 NOTIFY space250Changed)
        int getSpace250() const;
    Q_SIGNAL void space250Changed();

    Q_PROPERTY(int Space300 READ getSpace300 NOTIFY space300Changed)
        int getSpace300() const;
    Q_SIGNAL void space300Changed();

    Q_PROPERTY(int Space400 READ getSpace400 NOTIFY space400Changed)
        int getSpace400() const;
    Q_SIGNAL void space400Changed();

    Q_PROPERTY(int Space450 READ getSpace450 NOTIFY space450Changed)
        int getSpace450() const;
    Q_SIGNAL void space450Changed();

    Q_PROPERTY(int Space500 READ getSpace500 NOTIFY space500Changed)
        int getSpace500() const;
    Q_SIGNAL void space500Changed();

    Q_PROPERTY(int Space600 READ getSpace600 NOTIFY space600Changed)
        int getSpace600() const;
    Q_SIGNAL void space600Changed();

    Q_PROPERTY(int Space700 READ getSpace700 NOTIFY space700Changed)
        int getSpace700() const;
    Q_SIGNAL void space700Changed();

    Q_PROPERTY(int Space800 READ getSpace800 NOTIFY space800Changed)
        int getSpace800() const;
    Q_SIGNAL void space800Changed();

    Q_PROPERTY(int Space900 READ getSpace900 NOTIFY space900Changed)
        int getSpace900() const;
    Q_SIGNAL void space900Changed();

public:
    enum class BreakPointType : uint16_t {
        BreakPointCompact = 600,
        BreakPointMedium = 840,
        BreakPointExpanded = 1200,
        BreakPointLarge = 1600,
        BreakPointExtraLarge = UINT16_MAX
    };
    Q_ENUM(BreakPointType);

    Q_PROPERTY(uint16_t breakPointType READ getBreakPointType NOTIFY breakPointTypeChanged)
        uint16_t breakPointType = 0;
    uint16_t getBreakPointType() const;
    Q_SIGNAL void breakPointTypeChanged();

    Q_PROPERTY(uint16_t breakPointValue MEMBER breakPointValue NOTIFY breakPointValueChanged)
        uint16_t breakPointValue = 0;
    Q_SIGNAL void breakPointValueChanged();

    Q_PROPERTY(QList<uint16_t> breakPointList MEMBER breakPointList NOTIFY breakPointListChanged)
        QList<uint16_t> breakPointList = {
            (uint16_t)BreakPointType::BreakPointCompact,
            (uint16_t)BreakPointType::BreakPointMedium,
            (uint16_t)BreakPointType::BreakPointExpanded,
            (uint16_t)BreakPointType::BreakPointLarge,
            (uint16_t)BreakPointType::BreakPointExtraLarge
    };
    Q_SIGNAL void breakPointListChanged();
private:
    material_color_utilities::DynamicScheme* scheme = nullptr;
    void initScheme();
    void emitScheme();
};
