import QtQuick
import YT_Player

Canvas {
    id: root
    property int offset: parent.radius ?? 0
    property point inputPos: Qt.point(0, 0)
    property point outputPos: Qt.point(0, 0)

    z: +true
    x: Math.min(inputPos.x, outputPos.x) - offset
    y: Math.min(inputPos.y, outputPos.y) - offset
    width: Math.abs(outputPos.x - inputPos.x) + offset  * 2
    height: Math.abs(outputPos.y - inputPos.y) + offset * 2

    onPaint: {
        var ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)

        // 绘制矩形边框
        // ctx.beginPath()
        // ctx.rect(0, 0, width, height)
        // ctx.strokeStyle = "red" // 边框颜色
        // ctx.lineWidth = 2 // 边框宽度
        // ctx.stroke()

        // console.log(x, y, width, height, inputPos, outputPos)

        const inputPosPaint = mapFromItem(parent, inputPos)
        const outputPosPaint = mapFromItem(parent, outputPos)
        const inputPosControl = Qt.point(inputPosPaint.x - (inputPosPaint.x - outputPosPaint.x) * 0.5, inputPosPaint.y)
        const outputPosControl = Qt.point(outputPosPaint.x + (inputPosPaint.x - outputPosPaint.x) * 0.5, outputPosPaint.y)

        ctx.beginPath();
        ctx.moveTo(outputPosPaint.x, outputPosPaint.y);
        ctx.bezierCurveTo(outputPosControl.x, outputPosControl.y, inputPosControl.x, inputPosControl.y, inputPosPaint.x, inputPosPaint.y);
        ctx.strokeStyle = YT_Info.ItemFocusColor;
        ctx.lineWidth = 3;
        ctx.stroke();
    }
}
