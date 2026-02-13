#ifndef FILEWIDGET_H
#define FILEWIDGET_H

#include <YT_GeneralH.h>
#include <FileItemModel.h>

class FileWidget : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString operationType MEMBER operationType NOTIFY operationTypeChanged)
    Q_PROPERTY(QList<QString> operationPaths MEMBER operationPaths)

    Q_PROPERTY(QList<QString> curPath READ getCurPath NOTIFY curPathChanged)
    Q_PROPERTY(FileItemModel *curModel READ getCurModel WRITE setCurModel NOTIFY curModelChanged)
public:
    explicit FileWidget(QQuickItem *parent = nullptr);

    Q_INVOKABLE void create(const QString &path);
    Q_INVOKABLE void createFolder(const QString &path);
    Q_INVOKABLE void copy(const QString &path);
    Q_INVOKABLE void move(const QString &path);
    Q_INVOKABLE void rename(const QString &name);
    Q_INVOKABLE void remove();
protected:
    QString operationType = {};
    QList<QString> operationPaths = {};

    QList<QString> curPath;
    QList<QString> getCurPath() const { return curPath; }
    Q_INVOKABLE void setCurPath(const QString &data);
    Q_INVOKABLE void setCurPath(const QList<QString> &data);
    Q_INVOKABLE QList<QString> getSubPath(const QList<QString> &data) const;

    FileItemModel *curModel = nullptr;
    FileItemModel *getCurModel() const { return curModel; }
    void setCurModel(FileItemModel *&data) {
        if (curModel == data)
            return ;
        if (curModel)
            disconnect(curModel, &FileItemModel::currentDirChanged, this, &FileWidget::curModelPathChanged);
        if (data)
            connect(data, &FileItemModel::currentDirChanged, this, &FileWidget::curModelPathChanged);

        curModel = data;
        curModelPathChanged();
        emit curModelChanged();
    }
    void curModelPathChanged() {
        static const QRegularExpression re(R"([/\\])");
        curPath = curModel ? curModel->currentDir.split(re) : QList<QString>();
        curPath.removeAll("");
        emit curPathChanged();
    }
signals:
    void operationTypeChanged();
    void curPathChanged();
    void curModelChanged();
};

#endif // FILEWIDGET_H
