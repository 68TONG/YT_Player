import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT

FloatingActionButton {
    id: root

    enum StyleT {
        Primary,
        Secondary,
        Tertiary
    }
    sizeType: -1
    styleType: FloatingActionButtonMenu.Primary

    checkable: true
    spacing: 8
    implicitWidth: 56
    implicitHeight: 56
    radius: checked ? YT.Theme.RadiusFull : YT.Theme.RadiusLarge
    iconSize: 20

    default property alias contentData: container.contentData
    T.Popup {
        id: popup
        y: root.height + root.spacing
        padding: 0
        contentItem: T.Container {
            id: container
            contentItem: ListView {
                spacing: 4
                orientation: ListView.Vertical
                implicitWidth: popup.implicitSize
                implicitHeight: contentItem.height
                model: popup.visible ? container.contentModel : null

                populate: Transition {
                    id: populateTrans
                    SequentialAnimation {
                        PropertyAction {
                            properties: "opacity,scale"
                            value: 0.0
                        }
                        PropertyAction {
                            property: "transformOrigin"
                            value: Item.TopLeft
                        }
                        PauseAnimation {
                            duration: (populateTrans.ViewTransition.index - populateTrans.ViewTransition.targetIndexes[0]) * 100
                        }
                        NumberAnimation {
                            properties: "opacity,scale"
                            from: 0.0
                            to: 1.0
                            duration: 180
                            easing.type: Easing.OutCubic
                        }
                    }
                }
            }
        }
        background: null

        property real implicitSize: 0
        onAboutToShow: {
            let m_implicitSize = 0;
            for (var i = 0; i < container.contentModel.count; i++) {
                const it = container.contentModel.get(i);
                m_implicitSize = Math.max(m_implicitSize, it.implicitWidth);
            }
            implicitSize = m_implicitSize;
        }
        onClosed: {
            root.checked = false;
        }
    }

    onClicked: popup.open()
    Component.onCompleted: {
        for (let i = 0; i < container.contentModel.count; i++) {
            let it = container.contentModel.get(i);
            if (it instanceof M_Item) {
                it.m_parent = root;
            }
        }
    }

    component M_Item: YT.FloatingActionButtonEx {
        property FloatingActionButtonMenu m_parent: null

        styleType: {
            if (m_parent == null) {
                return YT.FloatingActionButtonEx.PrimaryContainer;
            }

            if (m_parent.styleType == FloatingActionButtonMenu.Primary) {
                return YT.FloatingActionButtonEx.PrimaryContainer;
            } else if (m_parent.styleType == FloatingActionButtonMenu.Secondary) {
                return YT.FloatingActionButtonEx.SecondaryContainer;
            } else if (m_parent.styleType == FloatingActionButtonMenu.Tertiary) {
                return YT.FloatingActionButtonEx.TertiaryContainer;
            }
            return YT.FloatingActionButtonEx.PrimaryContainer;
        }

        elevation: YT.Theme.Elevation0
        font: YT.Theme.FontSmall
        spacing: YT.Theme.Space100
        horizontalPadding: YT.Theme.Space300
        iconSize: 24
    }
}
