import QtQuick
import YT_Material as YT

ListView {
    id: root

    // property alias treeModel: treeProxyModel.m_sourceModel
    // property alias treeProxyModel: treeProxyModel

    // function depth(row) {
    //     return treeProxyModel.depth(row);
    // }
    // function expand(row) {
    //     return treeProxyModel.expand(row);
    // }
    // function collapse(row) {
    //     return treeProxyModel.collapse(row);
    // }
    // function isExpanded(row) {
    //     return treeProxyModel.isExpanded(row);
    // }
    // function toggleExpanded(row) {
    //     return treeProxyModel.toggleExpanded(row);
    // }

    // YT_AbstractTreeProxyModel {
    //     id: treeProxyModel
    //     m_sourceModel: root.model
    // }

    add: Transition {
        NumberAnimation {
            properties: "opacity"
            from: 0
            to: 1
            duration: 150
            easing.type: Easing.OutQuad
        }
    }
    remove: Transition {
        NumberAnimation {
            properties: "opacity"
            from: 1
            to: 0
            duration: 150
            easing.type: Easing.OutQuad
        }
    }
    displaced: Transition {
        NumberAnimation {
            properties: "y"
            duration: 180
            easing.type: Easing.OutQuad
        }
    }
}
