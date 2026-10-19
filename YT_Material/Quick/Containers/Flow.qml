pragma ComponentBehavior: Bound
import QtQuick

Item {
    id: root

    default property alias contentData: root.data

    property int flow: Flow.LeftToRight
    property real spacing: 0
    property real padding: 0
    property real leftPadding: padding
    property real topPadding: padding
    property real rightPadding: padding
    property real bottomPadding: padding
    implicitWidth: 0

    function scheduleLayout() {
        Qt.callLater(layoutChildren);
    }

    function layoutChildren() {
        const horizontal = root.flow === Flow.LeftToRight;
        const childItems = root.children;
        const entries = [];
        let totalMainSize = 0;
        let visibleCount = 0;

        for (let i = 0; i < childItems.length; ++i) {
            const item = childItems[i];
            if (!item.visible)
                continue;

            const mainSize = horizontal ? item.width : item.height;
            const crossSize = horizontal ? item.height : item.width;
            entries.push({
                item: item,
                index: i,
                mainSize: mainSize,
                crossSize: crossSize
            });
            totalMainSize += mainSize;
            ++visibleCount;
        }

        if (visibleCount > 1)
            totalMainSize += spacing * (visibleCount - 1);

        entries.sort(function (a, b) {
            if (a.mainSize !== b.mainSize)
                return b.mainSize - a.mainSize;
            if (a.crossSize !== b.crossSize)
                return b.crossSize - a.crossSize;
            return a.index - b.index;
        });

        const availableMainSize = horizontal
            ? root.width - root.leftPadding - root.rightPadding
            : root.height - root.topPadding - root.bottomPadding;
        const mainLimit = availableMainSize > 0 ? availableMainSize : totalMainSize;
        const rows = [];

        for (let i = 0; i < entries.length; ++i) {
            const entry = entries[i];
            let bestRow = -1;
            let bestRemaining = Number.POSITIVE_INFINITY;

            for (let rowIndex = 0; rowIndex < rows.length; ++rowIndex) {
                const row = rows[rowIndex];
                const extraSize = entry.mainSize + (row.items.length > 0 ? root.spacing : 0);
                const remaining = mainLimit - row.usedMain - extraSize;
                if (remaining >= 0 && remaining < bestRemaining) {
                    bestRow = rowIndex;
                    bestRemaining = remaining;
                }
            }

            if (bestRow < 0) {
                rows.push({
                    items: [],
                    usedMain: 0,
                    crossSize: 0
                });
                bestRow = rows.length - 1;
            }

            const targetRow = rows[bestRow];
            if (targetRow.items.length > 0)
                targetRow.usedMain += root.spacing;
            targetRow.items.push(entry);
            targetRow.usedMain += entry.mainSize;
            targetRow.crossSize = Math.max(targetRow.crossSize, entry.crossSize);
        }

        let implicitMainSize = 0;
        let implicitCrossSize = 0;
        let crossOffset = 0;

        for (let rowIndex = 0; rowIndex < rows.length; ++rowIndex) {
            const row = rows[rowIndex];
            if (rowIndex > 0)
                crossOffset += root.spacing;

            let mainOffset = 0;

            for (let itemIndex = 0; itemIndex < row.items.length; ++itemIndex) {
                const entry = row.items[itemIndex];
                if (itemIndex > 0)
                    mainOffset += root.spacing;

                if (horizontal) {
                    entry.item.x = root.leftPadding + mainOffset;
                    entry.item.y = root.topPadding + crossOffset;
                } else {
                    entry.item.x = root.leftPadding + crossOffset;
                    entry.item.y = root.topPadding + mainOffset;
                }
                mainOffset += entry.mainSize;
            }

            implicitMainSize = Math.max(implicitMainSize, row.usedMain);
            crossOffset += row.crossSize;
        }

        implicitCrossSize = crossOffset;

        root.implicitWidth = (horizontal ? implicitMainSize : implicitCrossSize) + root.leftPadding + root.rightPadding;
        root.implicitHeight = (horizontal ? implicitCrossSize : implicitMainSize) + root.topPadding + root.bottomPadding;
    }

    onFlowChanged: scheduleLayout()
    onSpacingChanged: scheduleLayout()
    onPaddingChanged: scheduleLayout()
    onLeftPaddingChanged: scheduleLayout()
    onTopPaddingChanged: scheduleLayout()
    onRightPaddingChanged: scheduleLayout()
    onBottomPaddingChanged: scheduleLayout()
    onWidthChanged: {
        if (flow === Flow.LeftToRight)
            scheduleLayout();
    }
    onHeightChanged: {
        if (flow === Flow.TopToBottom)
            scheduleLayout();
    }

    onChildrenChanged: scheduleLayout()

    Instantiator {
        model: root.children
        delegate: Connections {
            required property var modelData
            target: modelData
            function onWidthChanged() {
                root.scheduleLayout();
            }
            function onHeightChanged() {
                root.scheduleLayout();
            }
            function onVisibleChanged() {
                root.scheduleLayout();
            }
        }
    }

    Component.onCompleted: layoutChildren()
}
