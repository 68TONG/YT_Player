#include "YT_Window.h"

YT_Window::YT_Window()
{
    this->setColor(QColor(0, 0, 0, 0));
    // this->setFlags(Qt::Dialog);
    this->setFlags(Qt::Window | Qt::FramelessWindowHint);
    // this->setFlags(Qt::Window | Qt::ExpandedClientAreaHint | Qt::NoTitleBarBackgroundHint/* | Qt::CustomizeWindowHint*/);
}
