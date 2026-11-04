#pragma once

#include <m3shapes/qml/materialshapeitem.hpp>

class YT_MaterialShape : public m3shapes::MaterialShapeItem {
    Q_OBJECT
    QML_NAMED_ELEMENT(MaterialShape)
public:
    explicit YT_MaterialShape(QQuickItem* parent = nullptr);
};
