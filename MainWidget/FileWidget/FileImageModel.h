#pragma once

#include <QQuickImageProvider>
// #include <QFileIconProvider>

class FileImageModel : public QQuickImageProvider
{
    Q_OBJECT
public:
    explicit FileImageModel();

    virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override;
private:
    // QFileIconProvider fileIconProvider;
};
