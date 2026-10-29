import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQml.Models
import YT_Material as YT_M

Flow {
    id: root

    YT_M.Button {
        text: "打开菜单"
        onClicked: {
            recentFilesInstantiator.active = true;
            menu.open();
        }
    }

    YT_M.Menu {
        id: menu
        title: qsTr("File")
        height: 500

        Instantiator {
            id: recentFilesInstantiator
            model: FileModel.properties
            delegate: MenuItem {
                required property string modelData
                text: modelData
            }

            // onObjectAdded: (index, object) => menu.insertItem(index, object)
            onObjectAdded: (index, object) => menu.addItem(object)
            onObjectRemoved: (index, object) => menu.removeItem(object)
        }
        onAboutToHide: recentFilesInstantiator.active = false
    }
}
