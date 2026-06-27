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

    Q_INVOKABLE void centerToScreen();
    Q_INVOKABLE void centerToParent();
    Q_INVOKABLE void centerToWindow(QWindow* targetWindow);

protected:
signals:
};

#endif // YT_WINDOW_H
