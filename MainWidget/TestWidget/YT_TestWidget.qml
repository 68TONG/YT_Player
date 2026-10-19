pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import YT_Player

Item {
    id: root

    readonly property list<Item> navigationWidget_Items: [listView]
    Component.onCompleted: {
        YT.testWidget = this;
    }
    Loader {
        id: rootContent
        anchors.fill: parent
        source: listModel.get(listView.currentIndex).loaderSource
    }

    property alias listView: listView
    YT_NavigationWidget.M_View {
        id: listView

        currentIndex: 0
        model: listModel
        delegate: root.listDelegate
    }

    ListModel {
        id: listModel
        ListElement {
            modelData: "临时Test"
            loaderSource: "YT_TestWidget_Default.qml"
        }
        ListElement {
            modelData: "Flow"
            loaderSource: "YT_TestWidget_Flow.qml"
        }
        ListElement {
            modelData: "图标变体"
            loaderSource: "YT_TestWidget_IconVariableAxes.qml"
        }
        ListElement {
            modelData: "SpringAnimation演示"
            loaderSource: "YT_TestWidget_SpringAnimation.qml"
        }
    }

    readonly property Component listDelegate: YT_NavigationWidget.M_ViewItem {
        id: listDelegate
        required property string modelData
        m.text: modelData
        m.onClicked: root.listView.currentIndex = listDelegate.index
    }
}
