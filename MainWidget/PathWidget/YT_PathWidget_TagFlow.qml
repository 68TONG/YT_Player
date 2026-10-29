import QtQuick
import YT_Material as YT_M

Flow {
    id: root

    property alias model: rootInstantiator.model
    property alias delegate: rootInstantiator.delegate
    property bool expanded: false

    spacing: YT_M.Theme.Space25
    Instantiator {
        id: rootInstantiator
        onObjectAdded: function (index, object) {
            object.parent = root;
            object.opacity = Qt.binding(function () {
                return object.y == 0 || root.expanded;
            });
        }
        onObjectRemoved: function (index, object) {
            object.parent = null;
        }
    }
}
