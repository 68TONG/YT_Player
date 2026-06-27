#include "YT_ComputeSizeProxy.h"

YT_ComputeSizeProxy::YT_ComputeSizeProxy(QObject* parent)
    : QObject{ parent }
{
}

bool YT_ComputeSizeProxy::eventFilter(QObject* obj, QEvent* evt)
{
    if (obj != target)
        return QObject::eventFilter(obj, evt);
    if (evt->type() == QEvent::ChildAdded) {
        qDebug() << evt << obj;
    }
    else if (evt->type() == QEvent::ChildRemoved) {
        qDebug() << evt << obj;
    }
    qDebug() << evt << obj;

    return QObject::eventFilter(obj, evt);
}

QQuickItem* YT_ComputeSizeProxy::getTarget()
{
    return target;
}

void YT_ComputeSizeProxy::setTarget(QQuickItem* data)
{
    if (target) return;
    target = data;
    target->installEventFilter(this);
}