#include <YT_GeneralH.h>
#include <YT_Info.h>
#include <MusicImageModel.h>

#include <QQuickStyle>
#include <QLoggingCategory>

#include "VideoBuffer.h"
#include "MediaDecoder.h"

int main(int argc, char* argv[])
{
    QLoggingCategory::setFilterRules("qt.multimedia.*=false");
    QGuiApplication app(argc, argv);

    app.setFont(QFont("斜体", 10));
    QQuickStyle::setStyle("Basic");

    QQmlApplicationEngine engine;
    engine.addImageProvider(QLatin1String("MusicImage"), new MusicImageModel);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() {
        QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    QObject::connect(&app, &QGuiApplication::aboutToQuit, []() {
        YT_Info::getObject().aboutToQuit = true;
        });
    engine.loadFromModule("YT_Player", "Main");

    return app.exec();
}
