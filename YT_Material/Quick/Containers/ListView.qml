import QtQuick
import YT_Material as YT_M

ListView {
    id: root

    spacing: YT_M.Theme.Space25
    highlightFollowsCurrentItem: false
    displaced: Transition {
        NumberAnimation {
            properties: "x,y"
            duration: YT_M.Easing.Medium3
            easing: YT_M.Easing.Standard
        }
    }
    move: Transition {
        NumberAnimation {
            properties: "x,y"
            duration: YT_M.Easing.Medium3
            easing: YT_M.Easing.Standard
        }
    }
    populate: Transition {
        SequentialAnimation {
            PropertyAction {
                property: "opacity"
                value: 0.0
            }
            PauseAnimation {
                duration: {
                    const data = root.populate.ViewTransition;
                    Math.pow(data.index - data.targetIndexes[0], 0.4) * 120;
                }
            }
            ParallelAnimation {
                NumberAnimation {
                    property: "opacity"
                    from: 0.0
                    to: 1.0
                    duration: YT_M.Easing.Medium3
                    easing: YT_M.Easing.LegacyAccelerate
                }
                NumberAnimation {
                    property: "y"
                    from: root.populate.ViewTransition.destination.y + 120
                    duration: YT_M.Easing.Medium3
                    easing: YT_M.Easing.Legacy
                }
            }
        }
    }
}
