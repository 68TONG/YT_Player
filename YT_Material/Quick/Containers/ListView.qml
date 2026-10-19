import QtQuick
import YT_Material as YT

ListView {
    id: rootView

    spacing: YT.Theme.Space25
    highlightFollowsCurrentItem: false
    displaced: Transition {
        NumberAnimation {
            properties: "x,y"
            duration: 300
            easing.type: Easing.OutCubic
        }
    }
    populate: Transition {
        id: rootViewPopulate
        property int animationIndex: ViewTransition.index - ViewTransition.targetIndexes[0]
        onAnimationIndexChanged: (100 / (rootViewPopulate.animationIndex + 1)) + 50
        SequentialAnimation {
            PropertyAction {
                property: "opacity"
                value: 0.0
            }
            PauseAnimation {
                duration: Math.pow(rootViewPopulate.animationIndex, 0.4) * 120;
            }
            ParallelAnimation {
                NumberAnimation {
                    property: "opacity"
                    from: 0.0
                    to: 1.0
                    duration: 300
                    easing.type: Easing.OutCubic
                }
                NumberAnimation {
                    property: "y"
                    from: rootViewPopulate.ViewTransition.destination.y + 100
                    duration: 300
                    easing.type: Easing.OutCubic
                }
            }
        }
    }
    move: Transition {
        NumberAnimation {
            properties: "x,y"
            duration: 300
            easing.type: Easing.OutCubic
        }
    }
}
