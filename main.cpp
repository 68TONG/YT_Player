#include <YT_GeneralH.h>
#include <CoverCacheModel.h>

#include <QQuickStyle>
#include <QFontDatabase>
#include <QLoggingCategory>
#include <QWKQuick/qwkquickglobal.h>
#include <YT_Theme.h>
#include <YT_Info.h>
#include <YT_Func.h>

#include "VideoBuffer.h"
#include "MediaDecoder.h"

int main(int argc, char* argv[])
{
    QLoggingCategory::setFilterRules(QStringLiteral(
        "qt.multimedia.*=false\n"
        "qt.quick.image.*=false\n"
    ));

    QGuiApplication app(argc, argv);
    {
        QQuickStyle::setStyle("Basic");
        app.setFont(YT_Theme::obj().fontBodyMedium);
        YT_Func::getObject();
        YT_Info::getObject();
    }

    QQmlApplicationEngine engine;
    QWK::registerTypes(&engine);
    engine.addImportPath("./YT_Material");
    engine.addImageProvider(QLatin1String("CoverCache"), new CoverCacheModel);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() {
        QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    QObject::connect(&app, &QGuiApplication::aboutToQuit, []() {
        YT_Info::getObject().aboutToQuit = true;
        });
    engine.loadFromModule("YT_Player", "Main");

    // {
    //     QImage image = QImage("C:/TONG/Code/PixivDownload/output/令时焥夏KiriAINa/145441825.jpg");
    //     auto focus = YT_Func::getFileCoverVisualFocus(image);
    //     auto rect = YT_Func::getFileCoverVisualRect(16.0 / 9, image.size(), focus);
    //     YT_Func::outputFileCoverVisualRect(image, rect);
    //     qDebug() << focus << rect;
    // }
    return app.exec();
}
