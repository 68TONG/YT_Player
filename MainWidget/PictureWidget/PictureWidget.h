#ifndef PICTUREWIDGET_H
#define PICTUREWIDGET_H

#include <YT_GeneralH.h>

class PictureWidget : public QQuickItem
{
    Q_OBJECT
        QML_ELEMENT
public:
    explicit PictureWidget(QQuickItem* parent = nullptr);
};

#endif // PICTUREWIDGET_H
