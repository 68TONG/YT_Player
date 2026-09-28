#include <YT_GeneralH.h>
#include <YT_Info.h>
#include <MusicImageModel.h>
#include <FileImageModel.h>

#include <QQuickStyle>
#include <QFontDatabase>
#include <QLoggingCategory>
#include <QWKQuick/qwkquickglobal.h>
#include <YT_Theme.h>

#include "VideoBuffer.h"
#include "MediaDecoder.h"

int main(int argc, char* argv[])
{
    QLoggingCategory::setFilterRules("qt.multimedia.*=false");
    QGuiApplication app(argc, argv);

    // app.setFont(QFont("斜体", 10));
    {
        QQuickStyle::setStyle("Basic");
        app.setFont(YT_Theme::obj().fontBodyMedium);
    }

    QQmlApplicationEngine engine;
    QWK::registerTypes(&engine);
    engine.addImportPath("./YT_Material");
    engine.addImageProvider(QLatin1String("MusicImage"), new MusicImageModel);
    engine.addImageProvider(QLatin1String("FileImage"), new FileImageModel);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() {
        QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    QObject::connect(&app, &QGuiApplication::aboutToQuit, []() {
        YT_Info::getObject().aboutToQuit = true;
        });
    engine.loadFromModule("YT_Player", "Main");
    return app.exec();
}
