#ifndef FILEWIDGET_H
#define FILEWIDGET_H

#include <YT_GeneralH.h>
#include <FileItemModel.h>
#include <YT_PathWidgetModel.h>
#include <YT_SearchWidgetModel.h>

class FileWidget : public QQuickItem
{
    Q_OBJECT
        QML_ELEMENT

        Q_PROPERTY(QString operationType MEMBER operationType NOTIFY operationTypeChanged)
        Q_PROPERTY(QList<QString> operationPaths MEMBER operationPaths)
public:
    explicit FileWidget(QQuickItem* parent = nullptr);

    Q_INVOKABLE void create(const QString& path);
    Q_INVOKABLE void createFolder(const QString& path);
    Q_INVOKABLE void copy(const QString& path);
    Q_INVOKABLE void move(const QString& path);
    Q_INVOKABLE void rename(const QString& name);
    Q_INVOKABLE void remove();
protected:
    QString operationType = {};
    QList<QString> operationPaths = {};

    Q_PROPERTY(YT_PathWidgetModel* pathModel READ getPathModel CONSTANT)
        YT_PathWidgetModel pathModel;
    YT_PathWidgetModel* getPathModel() { return &pathModel; };

    Q_PROPERTY(YT_SearchWidgetModel* searchModel READ getSearchModel CONSTANT)
        YT_SearchWidgetModel searchModel;
    YT_SearchWidgetModel* getSearchModel() { return &searchModel; };

    Q_PROPERTY(FileItemModel* curModel READ getCurModel WRITE setCurModel NOTIFY curModelChanged)
        FileItemModel* curModel = nullptr;
    FileItemModel* getCurModel() const { return curModel; };
    void setCurModel(FileItemModel*& data);
    Q_SIGNAL void curModelChanged();
    void curModelPathChanged();
signals:
    void operationTypeChanged();
};

#endif // FILEWIDGET_H
