import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT

T.CheckBox {
    id: root

    padding: 0
    implicitWidth: parent ? parent.width : 0
    implicitHeight: implicitContentHeight + topPadding + bottomPadding

    property alias checkBox: checkBox
    indicator: YT.CheckBox {
        id: checkBox
        text: root.text
        checkState: root.checkState
        onCheckStateChanged: root.checkState = checkState
        
        anchors.top: root.top
        anchors.topMargin: root.topPadding
    }

    property alias buttonContainer: buttonContainer
    default property alias contentData: buttonContainer.contentData
    contentItem: YT.ButtonContainer {
        id: buttonContainer
        checkState: root.checkState
        onCheckStateChanged: root.checkState = checkState

        topPadding: checkBox.height + root.spacing
        leftPadding: containerDepth * containerIndentation

        exclusive: false
        orientation: Qt.Vertical
    }
    background: null
}
