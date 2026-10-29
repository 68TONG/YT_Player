import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT_M

T.Container {
    id: root

    property int sizeType: -1
    enum SizeT {
        Xsmall,
        Small,
        Medium,
        Large,
        Xlarge
    }
    
    property int styleType: -1
    property bool styleType_Round: true
    enum StyleT {
        Elevated,
        Filled,
        Tonal,
        Outlined,
        Text
    }

    implicitWidth: (orientation == Qt.Vertical ? root.containerSize : implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: (orientation == Qt.Horizontal ? root.containerSize : implicitContentHeight + topPadding + bottomPadding)

    property Item pressItem: null
    property Item pressItemL: null
    property Item pressItemR: null

    property alias view: rootView
    property alias orientation: rootView.orientation

    property alias exclusive: buttonGroup.exclusive
    property alias checkState: buttonGroup.checkState
    property alias checkedButton: buttonGroup.checkedButton
    property alias buttonGroup: buttonGroup
    T.ButtonGroup {
        id: buttonGroup
        buttons: root.contentChildren
    }

    Component.onCompleted: {
        for (let i = 0; i < root.contentModel.count; i++) {
            let it = root.contentModel.get(i);
            if (it instanceof M_Item) {
                it.buttonContainer = root;
            } else if (it instanceof CheckBoxContainer) {
                it.buttonContainer.containerDepth = Qt.binding(function () {
                    return root.containerDepth + 1;
                });
                it.buttonContainer.containerIndentation = Qt.binding(function () {
                    return root.containerIndentation;
                });
            }
        }
    }

    contentItem: ListView {
        id: rootView
        interactive: false
        spacing: root.spacing
        orientation: Qt.Horizontal
        implicitWidth: contentItem.width
        implicitHeight: contentItem.height
        model: root.contentModel
    }

    background: null

    spacing: {
        if (sizeType < 0) {
            return 0;
        }
        if (exclusive) {
            return 2;
        }
        if (sizeType == YT_M.Button.Xsmall) {
            return 18;
        } else if (sizeType == YT_M.Button.Small) {
            return 12;
        } else {
            return 8;
        }
        return 0;
    }
    property real innerRadius: {
        if (sizeType == ButtonContainer.Xlarge) {
            return 20;
        } else if (sizeType == ButtonContainer.Large) {
            return 16;
        } else {
            return 8;
        }
        return 0;
    }
    property real innerRadiusFocus: {
        if (sizeType == ButtonContainer.Xlarge) {
            return 16;
        } else if (sizeType == ButtonContainer.Large) {
            return 12;
        } else {
            return 4;
        }
        return 0;
    }

    property int containerDepth: 0
    property int containerIndentation: 12
    property real containerSize: {
        if (sizeType == ButtonContainer.Xsmall) {
            return 32;
        } else if (sizeType == ButtonContainer.Small) {
            return 40;
        } else if (sizeType == ButtonContainer.Medium) {
            return 56;
        } else if (sizeType == ButtonContainer.Large) {
            return 96;
        } else if (sizeType == ButtonContainer.Xlarge) {
            return 136;
        }
        if (parent == null) {
            return 0;
        }
        return orientation == Qt.Vertical ? parent.width : parent.height;
    }

    component M_Item: YT_M.Button {
        property ButtonContainer buttonContainer: null
        sizeType: buttonContainer ? buttonContainer.sizeType : YT_M.Button.Xsmall
        styleType: buttonContainer ? buttonContainer.styleType : YT_M.Button.Filled
        styleType_Round: buttonContainer ? buttonContainer.styleType_Round : true

        checkable: true
        property real leftRadius: {
            if (buttonContainer.checkedButton == this) {
                return YT_M.Theme.RadiusFull;
            }
            if (buttonContainer.exclusive == false) {
                return radius;
            }
            if (ObjectModel.index == 0) {
                return radius;
            }
            return pressed ? buttonContainer.innerRadiusFocus : buttonContainer.innerRadius;
        }
        property real rightRadius: {
            if (buttonContainer.checkedButton == this) {
                return YT_M.Theme.RadiusFull;
            }
            if (buttonContainer.exclusive == false) {
                return radius;
            }
            if (ObjectModel.index == buttonContainer.contentModel.count - 1) {
                return radius;
            }
            return pressed ? buttonContainer.innerRadiusFocus : buttonContainer.innerRadius;
        }

        topLeftRadius: leftRadius
        topRightRadius: rightRadius
        bottomLeftRadius: leftRadius
        bottomRightRadius: rightRadius

        onPressed: startPressAni()
        onReleased: endPressAni()
        onCanceled: endPressAni()

        function startPressAni() {
            if (buttonContainer.exclusive) {
                return;
            }

            let view = ListView.view;
            buttonContainer.pressItem = this;
            buttonContainer.pressItemL = view.itemAtIndex(ObjectModel.index - 1);
            buttonContainer.pressItemR = view.itemAtIndex(ObjectModel.index + 1);

            buttonContainer.pressItem.scale = 1.15;
            if (buttonContainer.pressItemL) {
                buttonContainer.pressItemL.transformOrigin = Item.Left;
                buttonContainer.pressItemL.scale = 0.95;
            }
            if (buttonContainer.pressItemR) {
                buttonContainer.pressItemR.transformOrigin = Item.Right;
                buttonContainer.pressItemR.scale = 0.95;
            }
        }
        function endPressAni() {
            if (buttonContainer.exclusive) {
                return;
            }

            let view = ListView.view;
            buttonContainer.pressItem.scale = 1.00;
            if (buttonContainer.pressItemL) {
                buttonContainer.pressItemL.transformOrigin = Item.Center;
                buttonContainer.pressItemL.scale = 1.00;
            }
            if (buttonContainer.pressItemR) {
                buttonContainer.pressItemR.transformOrigin = Item.Center;
                buttonContainer.pressItemR.scale = 1.00;
            }
        }

        Behavior on scale {
            NumberAnimation {
                easing: YT_M.Easing.SpringFastEx
                duration: YT_M.Easing.Medium3
            }
        }
    }
}
