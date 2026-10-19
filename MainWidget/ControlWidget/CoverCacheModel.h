#pragma once

#include <QQuickImageProvider>

class CoverCacheModel : public QQuickImageProvider
{
public:
    CoverCacheModel();
    QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override;
};
