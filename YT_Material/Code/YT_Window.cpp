#include "YT_Window.h"

#include <QWKQuick/quickwindowagent.h>
#include "YT_Theme.h"

YT_Window::YT_Window(QWindow* parent)
    :QQuickWindow(parent)
{
    this->setVisible(false);
    this->setColor(YT_Theme::obj().getBackground());
}

YT_Window::YT_Window(QQuickRenderControl* renderControl)
    :QQuickWindow(renderControl)
{
    this->setVisible(false);
    this->setColor(YT_Theme::obj().getBackground());
}

void YT_Window::setThemeBreakPoint()
{
    YT_Theme::obj().setBreakPointValue(this->width());
}

Q_INVOKABLE void YT_Window::connectThemeBreakPoint()
{
    connect(this, &YT_Window::widthChanged, this, &YT_Window::setThemeBreakPoint, (Qt::ConnectionType)(Qt::AutoConnection | Qt::UniqueConnection));
}

Q_INVOKABLE void YT_Window::disconnectThemeBreakPoint()
{
    disconnect(this, &YT_Window::widthChanged, this, &YT_Window::setThemeBreakPoint);
}

Q_INVOKABLE void YT_Window::initQuickWindowAgent()
{
    if (quickWindowAgent) return;
    if (this->isVisible()) this->setVisible(false);

    auto data = new QWK::QuickWindowAgent(this);
    data->setup(this);

    quickWindowAgent = data;
    this->setVisible(true);
}
