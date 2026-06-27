import QtQuick
import YT_Player

PictureWidget {
    id: root

    // ListModel {
    //     id: pictureModel
        
    //     Component.onCompleted: {
    //         // 示例数据 - 卡片数据
    //         pictureModel.append({
    //             id: 1,
    //             title: "雀死游戏港版吃。",
    //             date: "2026/06/23 17:54",
    //             imageIndex: 1,
    //             tags: ["悬疑", "TV", "战斗", "小说改"]
    //         })
    //         pictureModel.append({
    //             id: 2,
    //             title: "我们不可能起为芒人！绝对……",
    //             date: "2026/06/20 14:26",
    //             imageIndex: 2,
    //             tags: ["喜剧", "百合", "校园", "后宫", "TV"]
    //         })
    //         pictureModel.append({
    //             id: 3,
    //             title: "我们不可能起为芒人！绝对……",
    //             date: "2026/06/19 02:16",
    //             imageIndex: 3,
    //             tags: ["喜剧", "百合", "校园", "后宫", "TV"]
    //         })
    //         pictureModel.append({
    //             id: 4,
    //             title: "成神之日",
    //             date: "2026/06/14 15:23",
    //             imageIndex: 4,
    //             tags: ["TV", "恋爱", "原创", "日常"]
    //         })
    //         pictureModel.append({
    //             id: 5,
    //             title: "武装少女",
    //             date: "2026/06/12 16:28",
    //             imageIndex: 5,
    //             tags: ["校园", "后宫", "TV", "漫画改"]
    //         })
    //         pictureModel.append({
    //             id: 6,
    //             title: "樱花庄的宠物女孩",
    //             date: "2026/08/02 21",
    //             imageIndex: 1,
    //             tags: ["校园", "TV", "恋爱", "小说改", "青春"]
    //         })
    //         pictureModel.append({
    //             id: 7,
    //             title: "狂蛮之州××",
    //             date: "2026/05/30 23:41",
    //             imageIndex: 2,
    //             tags: ["百合", "校园", "漫画改"]
    //         })
    //         pictureModel.append({
    //             id: 8,
    //             title: "狂蛮之洲××",
    //             date: "2026/05/30 01:30",
    //             imageIndex: 3,
    //             tags: ["百合", "校园", "TV", "漫画改", "续作"]
    //         })
    //         pictureModel.append({
    //             id: 9,
    //             title: "狂蛮之洲",
    //             date: "2026/05/29 13:34",
    //             imageIndex: 4,
    //             tags: ["百合", "校园", "漫画改"]
    //         })
    //         pictureModel.append({
    //             id: 10,
    //             title: "山田君与7个魔女",
    //             date: "2026/05/28 16:48",
    //             imageIndex: 5,
    //             tags: ["校园", "后宫", "TV", "漫画改"]
    //         })
    //     }
    // }

    // GridView {
    //     id: gridView
    //     anchors.fill: parent
    //     anchors.margins: YT_Info.Margin
        
    //     model: pictureModel
    //     cellWidth: 200
    //     cellHeight: 320
        
    //     delegate: PictureCard {
    //         width: gridView.cellWidth - YT_Info.SpacingBig
    //         height: gridView.cellHeight - YT_Info.SpacingBig
            
    //         title: model.title
    //         date: model.date
    //         imageIndex: model.imageIndex
    //         tags: model.tags
    //     }
    // }

    // // 卡片组件
    // component PictureCard: Item {
    //     id: card
        
    //     required property string title
    //     required property string date
    //     required property int imageIndex
    //     required property list<string> tags
        
    //     YT_Rectangle {
    //         id: cardBackground
    //         anchors.fill: parent
    //         color: YT_Info.ItemColor
    //     }
        
    //     ColumnLayout {
    //         anchors.fill: parent
    //         anchors.margins: YT_Info.Margin
    //         spacing: YT_Info.SpacingSmall
            
    //         // 卡片图片
    //         YT_RoundImage {
    //             id: cardImage
    //             Layout.fillWidth: true
    //             Layout.preferredHeight: 160
    //             radius: YT_Info.RadiusBig
                
    //             source: "image://MusicImage/" + card.imageIndex
    //             asynchronous: true
                
    //             imageSource.onStatusChanged: {
    //                 if (status === Image.Error) {
    //                     console.warn("Failed to load image:", source);
    //                     // 使用备用颜色
    //                     cardImagePlaceholder.visible = true;
    //                 }
    //             }
    //         }
            
    //         // 图片加载失败时的占位符
    //         YT_Rectangle {
    //             id: cardImagePlaceholder
    //             Layout.fillWidth: true
    //             Layout.preferredHeight: 160
    //             visible: false
    //             color: YT_Info.ItemFocusColor
                
    //             YT_Text {
    //                 anchors.centerIn: parent
    //                 text: "图片加载失败"
    //                 font.pixelSize: 12
    //             }
    //         }
            
    //         // 卡片标题
    //         YT_Text {
    //             id: cardTitle
    //             Layout.fillWidth: true
    //             Layout.preferredHeight: 40
                
    //             text: card.title
    //             font.pixelSize: 14
    //             font.bold: true
    //             wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    //             maximumLineCount: 2
    //             elide: Text.ElideRight
    //         }
            
    //         // 观看日期
    //         YT_Text {
    //             id: cardDate
    //             Layout.fillWidth: true
    //             Layout.preferredHeight: 18
                
    //             text: "观看日期"
    //             font.pixelSize: 12
    //             color: YT_Info.FontColor
    //         }
            
    //         YT_Text {
    //             id: cardDateValue
    //             Layout.fillWidth: true
    //             Layout.preferredHeight: 18
                
    //             text: card.date
    //             font.pixelSize: 11
    //             color: YT_Info.FontFocusColor
    //         }
            
    //         // 文件标签
    //         Flow {
    //             Layout.fillWidth: true
    //             Layout.fillHeight: true
    //             spacing: YT_Info.SpacingSmall
                
    //             Repeater {
    //                 model: card.tags
                    
    //                 delegate: YT_Rectangle {
    //                     width: childrenRect.width + YT_Info.Margin * 2
    //                     height: 24
    //                     color: YT_Info.ItemFocusColor
    //                     radius: YT_Info.RadiusSmall
                        
    //                     YT_Text {
    //                         anchors.centerIn: parent
    //                         text: modelData
    //                         font.pixelSize: 10
    //                         color: YT_Info.FontColor
    //                     }
    //                 }
    //             }
    //         }
            
    //         Item {
    //             Layout.fillHeight: true
    //         }
    //     }
        
    //     // 鼠标悬停效果
    //     MouseArea {
    //         anchors.fill: parent
    //         hoverEnabled: true
            
    //         onEntered: {
    //             cardBackground.color = YT_Info.ItemFocusColor;
    //         }
    //         onExited: {
    //             cardBackground.color = YT_Info.ItemColor;
    //         }
    //     }
    // }
}
