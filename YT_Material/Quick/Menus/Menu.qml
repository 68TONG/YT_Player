pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic as T
import YT_Material as YT

T.Menu {
    id: root

    property variant mTarget: parent
    property int styleType: Menu.Standard
    enum StyleT {
        Vibrant,
        Standard
    }

    property bool hovered: false
    implicitWidth: m_implicitContentWidth + leftPadding + rightPadding
    implicitHeight: m_implicitContentHeight + topPadding + +bottomPadding
    verticalPadding: YT.Theme.Space50
    horizontalPadding: YT.Theme.Space50
    overlap: YT.Theme.Space25
    spacing: YT.Theme.Space25

    property int m_implicitContentCount: 5
    property real m_implicitContentWidth: parent ? parent.width : 0
    property real m_implicitContentHeight: isSeparatorGroup ? implicitContentHeight * Math.min(m_implicitContentCount, count) / count : implicitContentHeight
    property bool m_isGetImplicitContentSize: false
    onAboutToShow: {
        if (m_isGetImplicitContentSize == false)
            return;

        let delegateWidth = 0;
        let delegateHeight = -root.spacing;
        m_implicitContentWidth = 0;
        m_implicitContentHeight = 0;
        for (let i = 0; i < contentModel.count; i++) {
            const it = contentModel.get(i);
            delegateWidth = Math.max(delegateWidth, it.implicitWidth);
            delegateHeight = delegateHeight + it.implicitHeight + root.spacing;
        }
        m_implicitContentWidth = delegateWidth;
        m_implicitContentHeight = delegateHeight;
    }
    function m_open(target: Item) {
        parent = target;
        open();
    }

    readonly property bool isSeparatorGroup: separatorGroupModel.length == 0
    readonly property list<YT.MenuSeparatorGroup> separatorGroupModel: {
        let ret = [];
        for (let i = 0; i < contentModel.count; i++) {
            let it = contentModel.get(i);
            if (it instanceof YT.MenuSeparatorGroup)
                ret.push(it);
        }
        return ret;
    }

    delegate: MenuItem {}
    property Component m_delegate: null
    property alias model: instantiator.model
    Instantiator {
        id: instantiator

        model: null
        delegate: root.m_delegate != null ? root.m_delegate : root.delegate
        onObjectAdded: function (index, object) {
            root.addItem(object);
        }
        onObjectRemoved: function (index, object) {
            root.removeItem(object);
        }
    }

    contentItem: ListView {
        clip: true
        model: root.contentModel
        implicitWidth: contentItem/*.childrenRect*/.width
        implicitHeight: contentItem/*.childrenRect*/.height
        spacing: root.spacing

        HoverHandler {
            onHoveredChanged: {
                root.hovered = hovered;
            }
        }
    }

    background: Item {
        Loader {
            anchors.fill: parent
            active: root.isSeparatorGroup
            sourceComponent: Rectangle {
                color: root.styleType == Menu.Vibrant ? YT.Theme.TertiaryContainer : YT.Theme.SurfaceContainerLow
                radius: root.hovered ? YT.Theme.RadiusLarge : YT.Theme.RadiusSmall
                Behavior on radius {
                    SpringAnimation {
                        spring: 20
                        damping: 0.9
                    }
                }

                YT.ElevationEffect {
                    anchors.fill: parent
                    radius: parent.radius
                    elevation: YT.Theme.Elevation2
                }
            }
        }
        ColumnLayout {
            anchors.fill: parent
            spacing: YT.Theme.Space25
            Repeater {
                model: root.isSeparatorGroup == false ? root.separatorGroupModel.length + 1 : null
                delegate: Rectangle {
                    id: test
                    required property int index
                    readonly property YT.MenuSeparatorGroup previousItem: root.separatorGroupModel[index - 1] ?? null
                    readonly property YT.MenuSeparatorGroup currentItem: root.separatorGroupModel[index] ?? null

                    readonly property real offsetPos: root.topPadding
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredHeight: {
                        const pos = currentItem ? currentItem.y + offsetPos : root.implicitHeight;
                        const height = pos - (previousItem ? previousItem.y + previousItem.height + offsetPos : 0);
                        return height;
                    }

                    color: root.styleType == Menu.Vibrant ? YT.Theme.TertiaryContainer : YT.Theme.SurfaceContainerLow
                    radius: root.hovered ? YT.Theme.RadiusLarge : YT.Theme.RadiusSmall
                    topLeftRadius: previousItem ? YT.Theme.RadiusSmall : radius
                    topRightRadius: previousItem ? YT.Theme.RadiusSmall : radius
                    bottomLeftRadius: currentItem ? YT.Theme.RadiusSmall : radius
                    bottomRightRadius: currentItem ? YT.Theme.RadiusSmall : radius
                    Behavior on radius {
                        SpringAnimation {
                            spring: 20
                            damping: 0.9
                        }
                    }

                    YT.ElevationEffect {
                        anchors.fill: parent
                        anchors.topMargin: parent.currentItem == null ? YT.Theme.RadiusSmall : 0
                        anchors.bottomMargin: parent.previousItem == null ? YT.Theme.RadiusSmall : 0

                        radius: parent.previousItem && parent.currentItem ? YT.Theme.RadiusSmall : parent.radius
                        elevation: YT.Theme.Elevation2
                    }

                    Loader {
                        active: parent.previousItem == null || parent.currentItem == null

                        x: 0
                        y: parent.previousItem ? 0 : parent.height - height
                        z: -true
                        width: parent.width
                        height: YT.Theme.RadiusSmall * 2

                        sourceComponent: YT.ElevationEffect {
                            radius: YT.Theme.RadiusSmall
                            elevation: YT.Theme.Elevation2
                        }
                    }
                }
            }
        }
    }

    enter: Transition {
        NumberAnimation {
            property: "height"
            duration: 200
            from: 0.0
            to: root.implicitHeight
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            property: "opacity"
            duration: 120
            from: 0.0
            to: 1.0
            easing.type: Easing.OutQuad
        }
    }
    exit: Transition {
        NumberAnimation {
            property: "height"
            duration: 200
            from: root.height
            to: 0.0
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            property: "opacity"
            duration: 120
            from: 1.0
            to: 0.0
            easing.type: Easing.OutQuad
        }
    }
}
