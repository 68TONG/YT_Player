pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic as T
import YT_Material as YT_M

T.Menu {
    id: root

    property int styleType: Menu.Standard
    enum StyleT {
        Vibrant,
        Standard
    }

    property bool hovered: false
    implicitWidth: m_implicitContentWidth + leftPadding + rightPadding
    implicitHeight: m_implicitContentHeight + topPadding + +bottomPadding
    verticalPadding: YT_M.Theme.Space50
    horizontalPadding: YT_M.Theme.Space50
    overlap: YT_M.Theme.Space25
    spacing: YT_M.Theme.Space25

    property int m_implicitContentCount: 5
    property real m_implicitContentWidth: parent ? parent.width - leftPadding - rightPadding : 0
    property real m_implicitContentHeight: isSeparatorGroup ? implicitContentHeight * Math.min(m_implicitContentCount, count) / count : implicitContentHeight
    property bool m_isGetImplicitContentSize: false

    readonly property variant mTarget: parent
    property bool dynamicTarget: false
    function openTarget(data: Item) {
        dynamicTarget = true;
        parent = data;
        open();
    }
    property bool dynamicModel: false
    function openModel() {
        dynamicModel = true;
        instantiator.active = true;
        open();
    }
    onAboutToShow: {
        if (m_isGetImplicitContentSize == false)
            return;

        let delegateWidth = 0;
        let delegateHeight = -root.spacing;
        m_implicitContentWidth = 0;
        m_implicitContentHeight = 0;
        for (let i = 0; i < count; i++) {
            const it = itemAt(i);
            if (it == null)
                continue;
            delegateWidth = Math.max(delegateWidth, it.implicitWidth);
            delegateHeight = delegateHeight + it.implicitHeight + root.spacing;
        }
        m_implicitContentWidth = delegateWidth;
        m_implicitContentHeight = delegateHeight;
    }
    onAboutToHide: {
        if (dynamicTarget)
            parent = null;
        if (dynamicModel)
            instantiator.active = false;
    }

    readonly property bool isSeparatorGroup: separatorGroupModel.length == 0
    readonly property list<YT_M.MenuSeparatorGroup> separatorGroupModel: {
        let ret = [];
        for (let i = 0; i < count; i++) {
            const it = itemAt(i);
            if (it == null)
                continue;
            if (it instanceof YT_M.MenuSeparatorGroup)
                ret.push(it);
        }
        return ret;
    }

    delegate: MenuItem {}
    property Component mDelegate: null
    property alias model: instantiator.model
    Instantiator {
        id: instantiator
        model: null
        delegate: root.mDelegate != null ? root.mDelegate : root.delegate
        onObjectAdded: function (index, object) {
            root.addItem(object);
        }
        onObjectRemoved: function (index, object) {
            console.log(index, object);
            root.removeItem(object);
        }
    }

    contentItem: ListView {
        implicitWidth: contentItem/*.childrenRect*/.width
        implicitHeight: contentItem/*.childrenRect*/.height
        interactive: root.m_isGetImplicitContentSize == false
        spacing: root.spacing
        model: root.contentModel
        HoverHandler {
            onHoveredChanged: root.hovered = hovered
        }
    }

    background: Item {
        Loader {
            anchors.fill: parent
            active: root.isSeparatorGroup
            sourceComponent: Rectangle {
                color: root.styleType == Menu.Vibrant ? YT_M.Theme.TertiaryContainer : YT_M.Theme.SurfaceContainerLow
                radius: root.hovered ? YT_M.Theme.RadiusLarge : YT_M.Theme.RadiusSmall
                Behavior on radius {
                    SpringAnimation {
                        spring: 20
                        damping: 0.9
                    }
                }

                YT_M.ElevationEffect {
                    anchors.fill: parent
                    radius: parent.radius
                    elevation: YT_M.Theme.Elevation2
                }
            }
        }
        ColumnLayout {
            anchors.fill: parent
            spacing: YT_M.Theme.Space25
            Repeater {
                model: root.isSeparatorGroup == false ? root.separatorGroupModel.length + 1 : null
                delegate: Rectangle {
                    id: test
                    required property int index
                    readonly property YT_M.MenuSeparatorGroup previousItem: root.separatorGroupModel[index - 1] ?? null
                    readonly property YT_M.MenuSeparatorGroup currentItem: root.separatorGroupModel[index] ?? null

                    readonly property real offsetPos: root.topPadding
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredHeight: {
                        const pos = currentItem ? currentItem.y + offsetPos : root.implicitHeight;
                        const height = pos - (previousItem ? previousItem.y + previousItem.height + offsetPos : 0);
                        return height;
                    }

                    color: root.styleType == Menu.Vibrant ? YT_M.Theme.TertiaryContainer : YT_M.Theme.SurfaceContainerLow
                    radius: root.hovered ? YT_M.Theme.RadiusLarge : YT_M.Theme.RadiusSmall
                    topLeftRadius: previousItem ? YT_M.Theme.RadiusSmall : radius
                    topRightRadius: previousItem ? YT_M.Theme.RadiusSmall : radius
                    bottomLeftRadius: currentItem ? YT_M.Theme.RadiusSmall : radius
                    bottomRightRadius: currentItem ? YT_M.Theme.RadiusSmall : radius
                    Behavior on radius {
                        SpringAnimation {
                            spring: 20
                            damping: 0.9
                        }
                    }

                    YT_M.ElevationEffect {
                        anchors.fill: parent
                        anchors.topMargin: parent.currentItem == null ? YT_M.Theme.RadiusSmall : 0
                        anchors.bottomMargin: parent.previousItem == null ? YT_M.Theme.RadiusSmall : 0

                        radius: parent.previousItem && parent.currentItem ? YT_M.Theme.RadiusSmall : parent.radius
                        elevation: YT_M.Theme.Elevation2
                    }

                    Loader {
                        active: parent.previousItem == null || parent.currentItem == null

                        x: 0
                        y: parent.previousItem ? 0 : parent.height - height
                        z: -true
                        width: parent.width
                        height: YT_M.Theme.RadiusSmall * 2
                        sourceComponent: YT_M.ElevationEffect {
                            radius: YT_M.Theme.RadiusSmall
                            elevation: YT_M.Theme.Elevation2
                        }
                    }
                }
            }
        }
    }

    enter: Transition {
        NumberAnimation {
            property: "height"
            from: 0.0
            to: root.implicitHeight
            easing: YT_M.Easing.SpringFastEx
            duration: YT_M.Easing.Medium3
        }
        NumberAnimation {
            property: "opacity"
            from: 0.0
            to: 1.0
            easing: YT_M.Easing.StandardDecelerate
            duration: YT_M.Easing.Medium3
        }
    }
    exit: Transition {
        NumberAnimation {
            property: "height"
            from: root.height
            to: 0.0
            easing: YT_M.Easing.SpringFastEx
            duration: YT_M.Easing.Medium3
        }
        NumberAnimation {
            property: "opacity"
            from: 1.0
            to: 0.0
            easing: YT_M.Easing.StandardDecelerate
            duration: YT_M.Easing.Medium3
        }
    }
}
