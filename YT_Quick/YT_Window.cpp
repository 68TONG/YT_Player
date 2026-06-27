#include "YT_Window.h"

YT_Window::YT_Window()
{
    // this->setFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::NoTitleBarBackgroundHint);
    // this->setFlags(Qt::Window | Qt::ExpandedClientAreaHint | Qt::NoTitleBarBackgroundHint);
    // this->setFlags(Qt::Window | Qt::NoTitleBarBackgroundHint);
    this->setFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setColor(QColor(0, 0, 0, 0));
    this->setVisible(true);

    auto screen = QGuiApplication::primaryScreen();
    if (screen) {
        this->setWidth(screen->size().width() * 0.8);
        this->setHeight(screen->size().height() * 0.8);
    }
    centerToScreen();
}

void YT_Window::centerToScreen()
{
    QScreen* screen = this->screen();
    if (!screen)
        screen = QGuiApplication::primaryScreen();
    if (!screen)
        return;

    QRect screenGeometry = screen->availableGeometry();
    int x = screenGeometry.x() + (screenGeometry.width() - this->width()) / 2;
    int y = screenGeometry.y() + (screenGeometry.height() - this->height()) / 2;
    this->setPosition(x, y);
}

void YT_Window::centerToParent()
{
    QWindow* parentWindow = this->parent();
    if (parentWindow) {
        centerToWindow(parentWindow);
    }
    else {
        centerToScreen();
    }
}

void YT_Window::centerToWindow(QWindow* targetWindow)
{
    if (!targetWindow)
        return;

    QRect targetGeometry = targetWindow->geometry();
    int x = targetGeometry.x() + (targetGeometry.width() - this->width()) / 2;
    int y = targetGeometry.y() + (targetGeometry.height() - this->height()) / 2;
    this->setPosition(x, y);
}