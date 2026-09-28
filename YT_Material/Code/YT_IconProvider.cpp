#include "YT_IconProvider.h"

#include <QQmlEngine>
#include <QJSEngine>

YT_IconProvider::YT_IconProvider(QObject* parent)
    : QObject{ parent }
{
    {}
}

YT_IconProvider::~YT_IconProvider()
{
    {}
}

YT_IconProvider& YT_IconProvider::obj()
{
    static YT_IconProvider object;
    return object;
}

YT_IconProvider* YT_IconProvider::create(QQmlEngine*, QJSEngine*)
{
    QJSEngine::setObjectOwnership(&obj(), QJSEngine::CppOwnership);
    return &obj();
}
