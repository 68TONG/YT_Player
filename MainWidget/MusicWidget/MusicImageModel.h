#pragma once

#include <QQuickImageProvider>

class MusicImageModel : public QQuickImageProvider
{
    Q_OBJECT

public:
    explicit MusicImageModel();

    virtual QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;
signals:
};
