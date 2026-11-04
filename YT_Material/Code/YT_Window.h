#pragma once

#include "YT_Global.h"
#include <QQuickWindow>

class YT_EXPORT YT_Window : public QQuickWindow
{
    Q_OBJECT
        QML_NAMED_ELEMENT(Window)
public:
    explicit YT_Window(QWindow* parent = nullptr);
    explicit YT_Window(QQuickRenderControl* renderControl);

    void setThemeBreakPoint();
    Q_INVOKABLE void connectThemeBreakPoint();
    Q_INVOKABLE void disconnectThemeBreakPoint();
protected:
    QObject* quickWindowAgent = nullptr;
    Q_INVOKABLE void initQuickWindowAgent();
};
