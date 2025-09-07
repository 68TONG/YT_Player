import QtQuick
import QtQuick.Controls.Basic

import YT_Player

ItemSelectionModel {
    id: root

    required property var getItem_ModelIndex
    function selectItem(index = -1, modifiers = Qt.NoModifier) {
        var model_index = model.index(index, 0)
        if(model_index.isValid === false) {
            return clearSelection()
        }

        if(modifiers === Qt.NoModifier) {
            setCurrentIndex(model_index, ItemSelectionModel.ClearAndSelect)
        } else if(modifiers & Qt.ShiftModifier) {
            var current = currentIndex
            if(current.isValid === false) current = model_index

            var fromRow = Math.min(current.row, model_index.row)
            var toRow   = Math.max(current.row, model_index.row)

            if((modifiers & Qt.ControlModifier) !== Qt.ControlModifier)
                clearSelection()
            for (var i = fromRow; i <= toRow; ++i) {
                select(model.index(i, 0), ItemSelectionModel.Select)
            }
            setCurrentIndex(model_index, ItemSelectionModel.NoUpdate)
        } else if(modifiers & Qt.ControlModifier) {
            select(model_index, ItemSelectionModel.Toggle)
        }
    }

    // onSelectedIndexesChanged: console.log(selectedIndexes)
    onSelectionChanged: function(selected, deselected) {
        function rangesToRowIndexes(range_list) {
            var indexs = [];
            for (const range of range_list) {
                for (let r = range.top; r <= range.bottom; ++r) {
                    indexs.push(range.model.index(r, 0))
                }
            }
            return indexs;
        }


        {
            const index_list = rangesToRowIndexes(selected)
            for (const index of index_list) {
                var item = getItem_ModelIndex(index)
                if(item !== null && "selected" in item) item.selected = true
            }
        }

        {
            const index_list = rangesToRowIndexes(deselected)
            for (const index of index_list) {
                let item = getItem_ModelIndex(index)
                if(item !== null && "selected" in item) item.selected = false
            }
        }
    }
}
