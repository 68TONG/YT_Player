import QtQuick
import YT_Player

Rectangle {
    id: root
    property NE_Data parentNE_: null
    property Item viewNE_: null
    readonly property bool typeIO_: {
        if (parentNE_.outputNode === this)
            return true;
        if (parentNE_.inputNode === this)
            return false;
        console.error("Error: NE_Node != parentNE_.IO_Node");
        return undefined;
    }
    readonly property point centerPos: typeIO_ ? parentNE_.outputPos : parentNE_.inputPos

    x: centerPos.x - radius
    y: centerPos.y - radius
    width: radius * 2
    height: radius * 2

    radius: YT_Info.RadiusSmall * 2
    color: YT_Info.ItemFocusColor
    // color: Qt.rgba(Math.random(), Math.random(), Math.random(), 1) // 随机颜色

    Repeater {
        model: typeIO_ ? parentNE_.outputData : null
        delegate: NE_NodeLine {
            required property NE_Data modelData
            readonly property NE_Data inputData: modelData
            readonly property NE_Data outputData: parentNE_
            visible: viewNE_.dragInfo.dataNE_Reset !== inputData

            Component.onCompleted: {
                outputData._NE_RectChanged.connect(updateNodePos);
                outputData.outputPosChanged.connect(updateNodePos);

                inputData._NE_RectChanged.connect(updateNodePos);
                inputData.inputPosChanged.connect(updateNodePos);

                updateNodePos();
            }
            Component.onDestruction: {
                outputData._NE_RectChanged.disconnect(updateNodePos);
                outputData.outputPosChanged.disconnect(updateNodePos);

                if (inputData)
                    inputData._NE_RectChanged.disconnect(updateNodePos);
                if (inputData)
                    inputData.inputPosChanged.disconnect(updateNodePos);
            }

            function updateNodePos() {
                inputPos = YT.mapFromGlobal(root, inputData.inputNode.parent, inputData.inputPos);
                outputPos = YT.mapFromGlobal(root, outputData.outputNode.parent, outputData.outputPos);
            }
        }
    }

    DropArea {
        id: dropArea
        anchors.fill: parent
        anchors.margins: -root.radius
        onEntered: function (drag) {
            const target = drag.source;
            if (isAcceptNode(target) === false)
                return;
            target.dataNE_Connect = parentNE_;

            const nodeLine = viewNE_.nodeLine;
            if (typeIO_)
                nodeLine.outputPos = YT.mapFromGlobal(nodeLine, root.parent, centerPos);
            else
                nodeLine.inputPos = YT.mapFromGlobal(nodeLine, root.parent, centerPos);
        }
        onExited: function () {
            viewNE_.dragInfo.dataNE_Connect = null;
        }
        function isAcceptNode(data) {
            if (data.dataNE_Reset === parentNE_)
                return true;
            if (data.typeIO_ === typeIO_)
                return false;
            if (data.dataNE_ === parentNE_)
                return false;
            if (parentNE_.isconnectData(data.dataNE_))
                return false;
            return true;
        }
    }

    DragHandler {
        id: dragHandler
        target: viewNE_.dragInfo
        dragThreshold: 0
        grabPermissions: PointerHandler.TakeOverForbidden

        onActiveChanged: {
            var nodeLine = viewNE_.nodeLine;
            if (active === false) {
                var _type = target.typeIO_;
                var _data = target.dataNE_;
                var reset_data = target.dataNE_Reset;
                var connect_data = target.dataNE_Connect;

                if (connect_data === reset_data) {
                    ;
                } else if (connect_data !== null && reset_data !== null) {
                    _data.connectData(_type, connect_data);
                    _data.disconnectData(_type, reset_data);
                } else if (connect_data !== null && reset_data === null) {
                    _data.connectData(_type, connect_data);
                } else if (connect_data === null && reset_data !== null) {
                    _data.disconnectData(_type, reset_data);
                }

                target.Drag.drop();
                target.dataNE_ = null;
                target.dataNE_Reset = null;
                target.dataNE_Connect = null;
            } else if (typeIO_ === false && parentNE_.inputData !== null) {
                var inputData = parentNE_;
                var outputData = parentNE_.inputData;
                nodeLine.inputPos = YT.mapFromGlobal(nodeLine, inputData.inputNode.parent, inputData.inputPos);
                nodeLine.outputPos = YT.mapFromGlobal(nodeLine, outputData.outputNode.parent, outputData.outputPos);
                target.typeIO_ = true;
                target.dataNE_ = outputData;
                target.dataNE_Reset = inputData;
            } else {
                const io_pos = YT.mapFromGlobal(nodeLine, root.parent, centerPos);
                nodeLine.inputPos = io_pos;
                nodeLine.outputPos = io_pos;
                target.typeIO_ = typeIO_;
                target.dataNE_ = parentNE_;
                target.dataNE_Reset = null;
            }
            target.x = centroid.position.x;
            target.y = centroid.position.y;
            nodeLine.visible = active;
            nodeLine.requestPaint();
        }
        onCentroidChanged: {
            var nodeLine = viewNE_.nodeLine;
            if (target.dataNE_Connect)
                return;
            if (target.typeIO_)
                nodeLine.inputPos = YT.mapFromGlobal(nodeLine, root, centroid.position);
            else
                nodeLine.outputPos = YT.mapFromGlobal(nodeLine, root, centroid.position);
        }
    }
}
