#include <YT_GeneralH.h>
#include <CoverCacheModel.h>

#include <QQuickStyle>
#include <QFontDatabase>
#include <QLoggingCategory>
#include <YT_Font.h>
#include <YT_Theme.h>
#include <YT_Info.h>
#include <YT_Func.h>

int main(int argc, char* argv[])
{
    QLoggingCategory::setFilterRules(QStringLiteral(
        "qt.multimedia.*=false\n"
        "qt.quick.image.*=false\n"
    ));

    QGuiApplication app(argc, argv);
    {
        QQuickStyle::setStyle("Basic");
        app.setFont(YT_Font::obj().bodyMedium);
        YT_Func::getObject();
        YT_Info::getObject();
    }

    QQmlApplicationEngine engine;
    engine.addImageProvider(QLatin1String("CoverCache"), new CoverCacheModel);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() {
        QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    QObject::connect(&app, &QGuiApplication::aboutToQuit, []() {
        YT_Info::getObject().aboutToQuit = true;
        });
    engine.loadFromModule("YT_Player", "Main");

    return app.exec();
}
