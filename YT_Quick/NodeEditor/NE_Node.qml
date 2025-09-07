import QtQuick
import QtQuick.Controls

import YT_Player

Rectangle {
    width: 30; height: 30
    radius: width / 2
    color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemColor)
    border.width: 1
    border.color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemColor)
}
