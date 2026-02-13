import QtQuick
import QtQuick.Controls
import YT_Player

ApplicationWindow {
    id: root
    x: transientParent.x - width - 5
    y: transientParent.y + transientParent.height / 2 - height / 2
    width: 36
    height: mainLogo.view.contentHeight + mainLogo.view.topMargin + mainLogo.view.bottomMargin

    visible: true
    color: Qt.rgba(0, 0, 0, 0)
    flags: Qt.Window | Qt.FramelessWindowHint

    YT_ListView {
        id: mainLogo
        anchors.fill: parent
        radius: root.width / 2
        view.spacing: 7
        view.topMargin: 4
        view.leftMargin: 4
        view.rightMargin: 4
        view.bottomMargin: 4
        view.interactive: false
        view.currentIndex: YT.widgetIndex
        view.onCurrentIndexChanged: YT.widgetIndex = view.currentIndex
        followBackground.followItem: view.currentItem
        followBackground.radius: followBackground.width / 2

        model: YT.widgetModel.count
        delegate: YT_Button {
            required property int index
            readonly property QtObject model: YT.widgetModel.get(index)
            readonly property string logo_path: model.logo_path

            property int implicitSize: parent.width
            implicitWidth: implicitSize
            implicitHeight: implicitSize
            padding: 5

            modelData: logo_path
            contentItem: YT_Button.LoadImage {}
            background: null

            onClicked: YT.widgetIndex = index
        }
    }
}
