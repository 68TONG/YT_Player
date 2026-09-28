#pragma once

#include <QQuickImageProvider>

class MusicImageModel : public QQuickImageProvider
{
    Q_OBJECT
public:
    explicit MusicImageModel();
    virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override;
private: 
    QString outputCoverCache(const int info_id);
    inline static const QString cacheRootPath = "./YT_PlayerCache/MusicImage/";
};
