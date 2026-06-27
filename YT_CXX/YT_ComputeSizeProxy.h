#pragma once

#include <YT_GeneralH.h>

class YT_ComputeSizeProxy : public QObject
{
    Q_OBJECT
public:
    explicit YT_ComputeSizeProxy(QObject* parent = nullptr);

protected:
    bool eventFilter(QObject* obj, QEvent* evt) override;

    Q_PROPERTY(QQuickItem* target READ getTarget WRITE setTarget REQUIRED FINAL)
        QQuickItem* target = nullptr;
    QQuickItem* getTarget();
    void setTarget(QQuickItem* data);
};
