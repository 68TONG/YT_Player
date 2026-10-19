pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import YT_Player

Item {
    id: root

    readonly property list<Item> navigationWidget_Items: [listView]
    Component.onCompleted: {
        YT.setWidget = this;
    }

    StackLayout {
        id: rootContent
        anchors.fill: parent
        currentIndex: listView.currentIndex

        Loader {
            active: root.visible && rootContent.currentIndex === 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            sourceComponent: root.themeColorPage
        }
    }

    property alias listView: listView
    YT_NavigationWidget.M_View {
        id: listView

        currentIndex: 0
        model: ["主题颜色"]
        delegate: root.listDelegate
    }

    readonly property Component listDelegate: YT_NavigationWidget.M_ViewItem {
        id: listDelegate
        required property string modelData
        m.text: modelData
        m.onClicked: root.listView.currentIndex = listDelegate.index
    }

    readonly property Component themeColorPage: YT_SetWidget_ThemeColor {}
}
