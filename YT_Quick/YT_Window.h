#ifndef YT_WINDOW_H
#define YT_WINDOW_H

#include <YT_GeneralH.h>
#include <QQuickWindow>

class YT_Window : public QQuickWindow
{
    Q_OBJECT
    QML_ELEMENT
public:
    YT_Window();
protected:
signals:
};

#endif // YT_WINDOW_H
