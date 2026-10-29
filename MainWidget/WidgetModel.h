#pragma once

#include <QQuickItem>
#include <QAbstractItemModel>
#include "YT_VariantListModel.h"

class WidgetModel : public QAbstractItemModel
{
    Q_OBJECT
        QML_ELEMENT
        QML_SINGLETON
public:
    enum WidgetType {
        WidgetType_File,
        WidgetType_Music,
        WidgetType_Video,
        WidgetType_Picture,
        WidgetType_Creator,
        WidgetType_Edit,
        WidgetType_Set,
        WidgetType_Test
    };
    Q_ENUM(WidgetType)
public:
    enum WidgetRole {
        TypeRole,
        TitleRole,
        LogoIconRole,
        ComponentRole
    };
    Q_ENUM(WidgetRole)
private:
    explicit WidgetModel(QObject* parent = nullptr);
public:
    static WidgetModel& obj();
    static WidgetModel* create(QQmlEngine* qml_engine, QJSEngine*);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QVariant data_Index(const int& index, int role = Qt::DisplayRole) const;
public:
    Q_INVOKABLE QString getPath() const;
    Q_INVOKABLE QString getPathEx() const;
    Q_INVOKABLE QList<QString> getPathData() const;
    Q_INVOKABLE void setPath(int index);
    Q_INVOKABLE void setPath(QString data);
    Q_INVOKABLE void setPath(QList<QString> data);
    Q_INVOKABLE void addPath(QString data);
    Q_INVOKABLE void addPath(int index, QString data);
    Q_INVOKABLE QList<QString> getSubPath(int index) const;

    WidgetType getWidgetType() const;
    void setWidgetType(const WidgetType& data);
protected:
    QList<QVariantList> widgetDatas;
    Q_PROPERTY(WidgetType widgetType READ getWidgetType WRITE setWidgetType NOTIFY widgetTypeChanged)
        WidgetType widgetType = WidgetType_File;
    Q_SIGNAL void widgetTypeChanged();

    Q_PROPERTY(YT_VariantListModel* pathModel READ getPathModel CONSTANT)
        YT_VariantListModel pathModel;
    YT_VariantListModel* getPathModel() { return &pathModel; };
signals:
    void pathModelDataChanged();
};
