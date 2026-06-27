#include "PictureWidget.h"
#include <QStandardPaths>
#include <QDir>

PictureWidget::PictureWidget(QQuickItem* parent)
    : QQuickItem{ parent }
{
    this->setFlag(QQuickItem::ItemHasContents, true);
}