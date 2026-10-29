pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic
import YT_Player
import YT_Material as YT_M

GridView {
    id: root
    required property Menu menu

    readonly property list<string> properties: FileModel.selectProperty.filter(p => filterProperties.includes(p) == true)
    readonly property list<string> propertiesEx: FileModel.selectProperty.filter(p => filterProperties.includes(p) == false)
    readonly property list<string> filterProperties: {
        let ret = [];
        ret.push(FileModel.dataRoleName(FileModel.FileSize_Role));
        ret.push(FileModel.dataRoleName(FileModel.FileType_Role));
        ret.push(FileModel.dataRoleName(FileModel.FileCreationDate_Role));
        ret.push(FileModel.dataRoleName(FileModel.FileUpdatedDate_Role));
        ret.push(FileModel.dataRoleName(FileModel.StarLevel_RoleEx));
        ret.push(FileModel.dataRoleName(FileModel.PlayCount_RoleEx));
        ret.push(FileModel.dataRoleName(FileModel.AddPlayCount_RoleEx));
        return ret;
    }

    property Item expandItem: null
    property real imageImplicitHeight: 200 * (9 / 16)
    cellWidth: width / Math.floor(width / 200)
    cellHeight: (200 * 9 / 16) + (24 * 3) + (1) + (YT_M.Theme.Space100 * 2) + (YT_M.Theme.Space25 * 3)
    delegate: YT_M.Card {
        id: rootDelegate
        required property int index
        required property string infoTitle
        required property string filePath
        readonly property bool expanded: GridView.view.expandItem == this // qmllint disable missing-property

        z: expanded || itemCardDelegate_ExpandAnimation.running
        width: GridView.view.cellWidth
        height: expanded ? implicitHeight : GridView.view.cellHeight
        scale: expanded ? 1.05 : 1.0
        inset: YT_M.Theme.Space75
        spacing: YT_M.Theme.Space25
        topPadding: YT_M.Theme.Space150
        bottomPadding: YT_M.Theme.Space150
        horizontalPadding: YT_M.Theme.Space125
        elevation: {
            if (expanded) {
                return YT_M.Theme.Elevation3;
            } else if (draging) {
                return YT_M.Theme.Elevation3;
            } else if (pressed) {
                return YT_M.Theme.Elevation0;
            } else if (hovered) {
                return YT_M.Theme.Elevation1;
            } else {
                return YT_M.Theme.Elevation0;
            }
        }
        Behavior on height {
            NumberAnimation {
                id: itemCardDelegate_ExpandAnimation
                easing: YT_M.Easing.SpringFastEx
                duration: YT_M.Easing.Medium3
            }
        }
        Behavior on scale {
            NumberAnimation {
                easing: YT_M.Easing.SpringFastEx
                duration: YT_M.Easing.Medium3
            }
        }

        indicator: null
        contentItem: Column {
            spacing: rootDelegate.spacing
            topPadding: root.imageImplicitHeight
            Text {
                width: parent.width
                height: YT_M.Font.TitleMediumLineHeight
                font: YT_M.Font.TitleMedium
                text: rootDelegate.infoTitle
                elide: Text.ElideRight
                color: YT_M.Theme.OnSurface
                rightPadding: rootDelegate_ExpandButton.width
                YT_M.IconButton {
                    id: rootDelegate_ExpandButton
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    checkable: true
                    checked: rootDelegate.expanded
                    iconSize: rootDelegate.iconSize
                    iconSource: checked ? YT_M.IconProvider.UnfoldLess : YT_M.IconProvider.UnfoldMore
                    implicitHeight: iconSize
                    onToggled: {
                        root.expandItem = checked ? rootDelegate : null;
                    }
                }
            }

            YT_PathWidget_TagFlow {
                expanded: rootDelegate.expanded
                width: parent.width
                height: implicitHeight
                model: root.properties
                delegate: YT_PathWidget_TagButton {
                    required property string modelData
                    text: FileModel.data_Index(rootDelegate.index, modelData)
                    font: YT_M.Font.LabelMedium
                    labelLineHeight: YT_M.Font.LabelMediumLineHeight
                }
            }

            Instantiator {
                // asynchronous: true
                model: rootDelegate.expanded ? root.propertiesEx : root.propertiesEx.slice(0, 1)
                delegate: Column {
                    required property string modelData
                    width: parent ? parent.width : 0
                    height: implicitHeight
                    spacing: rootDelegate.spacing
                    Rectangle {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: parent.width - YT_M.Theme.Space200
                        height: 1
                        color: YT_M.Theme.OutlineVariant
                    }
                    YT_PathWidget_TagFlow {
                        expanded: rootDelegate.expanded
                        width: parent.width
                        height: implicitHeight
                        model: FileModel.data_Index(rootDelegate.index, parent.modelData) || ["<No Data>"]
                        delegate: YT_PathWidget_TagButton {
                            required property string modelData
                            text: modelData
                            onClicked: {
                                WidgetModel.addPath(modelData);
                            }
                        }
                    }
                }
                onObjectAdded: function (index, object) {
                    object.parent = rootDelegate.contentItem;
                }
                onObjectRemoved: function (index, object) {
                    object.parent = null;
                }
            }
        }

        background: YT_M.Background {
            initProperty: rootDelegate
            YT_M.RoundImage {
                readonly property variant coverInfo: YT_Func.getFileCoverInfo(rootDelegate.filePath)
                readonly property point visualFocus: coverInfo.visualFocus ?? Qt.point(0, 0)
                source: coverInfo.modelIndex
                asynchronous: true
                width: parent.width
                height: root.imageImplicitHeight
                radius: parent.radius
                fillMode: Image.Pad
                imageClipRect: YT_Func.getFileCoverVisualRect(width / height, Qt.size(implicitWidth, implicitHeight), visualFocus)
            }
        }

        onClickedEx: function (eventPoint, button, modifiers) {
            if (button == Qt.LeftButton) {
                console.log(root.properties, root.propertiesEx);
            } else if (button == Qt.RightButton) {
                root.menu.popup(rootDelegate);
            }
        }
        // onDoubleClicked: {
        //     if (root.itemModel.dataIndex(itemCardDelegate.index, FileItemModel.FileType_Role) == "Folder") {
        //         root.curListInfoPath = root.itemModel.dataIndex(itemCardDelegate.index, FileItemModel.FilePath_Role);
        //     }
        // }
    }
}
