import QtQuick

ListModel {
    id: root

    enum FilterType {
        Equals,
        Contains,
        StartsWith
    }

    property int filter_type: YT_FilterModel.Contains
    property var filter_data: null
    property var filter_model: []

    onFilter_modelChanged: filterFunc()
    onFilter_dataChanged: filterFunc()

    function filterFunc() {
        root.clear();
        if (filter_model == null)
            return;

        if (filter_data == null || filter_data == "") {
            for (var i = 0; i < filter_model.length; i++) {
                root.append({
                    "modelData": filter_model[i]
                });
            }
            return;
        }

        for (var i = 0; i < filter_model.length; i++) {
            var item = filter_model[i];
            var match = false;

            switch (filter_type) {
            case YT_FilterModel.Equals:
                match = (item == filter_data);
                break;
            case YT_FilterModel.Contains:
                match = (String(item).indexOf(String(filter_data)) !== -1);
                break;
            case YT_FilterModel.StartsWith:
                match = (String(item).startsWith(String(filter_data)));
                break;
            }

            if (match) {
                root.append({
                    "modelData": item
                });
            }
        }
    }
}
