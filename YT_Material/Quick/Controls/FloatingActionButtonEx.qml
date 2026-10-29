pragma ComponentBehavior: Bound
import QtQuick
import YT_Material as YT_M

FloatingActionButton {
    id: root

    indicator: Loader {
        active: root.text != ""
        anchors.left: root.left
        anchors.leftMargin: root.leftPadding + root.iconSize + root.spacing
        anchors.verticalCenter: root.verticalCenter
        sourceComponent: Text {
            text: root.text
            font: root.font
            color: root.labelColor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
    }
    contentItem: YT_M.Icon {
        iconSize: root.iconSize
        iconSource: root.iconSource
        iconColor: root.labelColor
        rightPadding: root.indicator && root.indicator.width > 0 ? root.indicator.width + root.spacing : 0
    }

    font: {
        if (sizeType == FloatingActionButton.Default) {
            YT_M.Font.TitleMedium;
        } else if (sizeType == FloatingActionButton.Medium) {
            return YT_M.Font.TitleLarge;
        } else if (sizeType == FloatingActionButton.Large) {
            return YT_M.Font.HeadlineSmall;
        }
        return undefined;
    }
    spacing: {
        if (sizeType == FloatingActionButton.Default) {
            return YT_M.Theme.Space100;
        } else if (sizeType == FloatingActionButton.Medium) {
            return YT_M.Theme.Space150;
        } else if (sizeType == FloatingActionButton.Large) {
            return YT_M.Theme.Space200;
        }
        return 0;
    }

    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    verticalPadding: {
        return 0;
    }
    horizontalPadding: {
        if (sizeType == FloatingActionButton.Default) {
            return 16;
        } else if (sizeType == FloatingActionButton.Medium) {
            return 26;
        } else if (sizeType == FloatingActionButton.Large) {
            return 28;
        }
        return 0;
    }
}
