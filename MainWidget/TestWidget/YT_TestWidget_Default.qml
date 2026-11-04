import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQml.Models
import YT_Material as YT_M

Flow {
    id: root
    spacing: 10
    YT_M.LoadingIndicator {
        id: shape
        implicitSize: 200
    }
    YT_M.LoadingIndicator {
        implicitSize: 200
        containerVisible: true
    }
    YT_M.LoadingIndicator {
        implicitSize: 200
        running: false
        containerVisible: true
    }
    YT_M.LoadingIndicator {
        id: load_item
        implicitSize: 200
        running: midpointSlider.pressed == false
        containerVisible: true
    }
    YT_M.ProgressIndicator {}
    YT_M.ProgressIndicator {
        implicitSize: 8
    }
    Column {
        width: 320
        spacing: 8

        Text {
            color: YT_M.Theme.OnSurface
            text: "Slider - minimum (" + minimumSlider.value + ")"
        }
        YT_M.Slider {
            id: minimumSlider
            width: parent.width
            from: 0
            to: 100
            value: 0
            stepSize: 10
            snapMode: Slider.SnapAlways
        }

        Text {
            color: YT_M.Theme.OnSurface
            text: "Slider - midpoint (" + midpointSlider.value + ")"
        }
        YT_M.Slider {
            id: midpointSlider
            width: parent.width
            from: 0.0
            to: 1.0
            onValueChanged: {
                load_item.progress = value;
            }
        }

        Text {
            color: YT_M.Theme.OnSurface
            text: "Slider - maximum (" + maximumSlider.value + ")"
        }
        YT_M.Slider {
            id: maximumSlider
            width: parent.width
            from: 0
            to: 100
            value: 100
            stepSize: 10
        }
    }
}
