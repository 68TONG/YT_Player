#include "YT_GeneralH.h"

QJsonObject readJsonObjectFromFile(const QString& file_path)
{
    QFile file(file_path);
    if (file.open(QIODevice::ReadOnly) == false)
    {
        qDebug() << "无法打开文件：" << file_path;
        return QJsonObject(); // 返回空的 QJsonObject
    }

    QByteArray file_data = file.readAll(); // 读取文件内容
    file.close();                          // 关闭文件

    QJsonParseError parse_error;
    QJsonDocument document = QJsonDocument::fromJson(file_data, &parse_error); // 解析 JSON 数据

    if (parse_error.error != QJsonParseError::NoError)
    {
        qDebug() << "JSON 解析错误：" << parse_error.errorString();
        return QJsonObject(); // 返回空的 QJsonObject
    }

    if (document.isObject() == false)
    {
        qDebug() << "JSON 数据不是一个对象";
        return QJsonObject(); // 返回空的 QJsonObject
    }

    return document.object(); // 返回解析后的 QJsonObject
}

void writeJsonObjectToFile(const QString& file_path, const QJsonObject& json_object)
{
    QFile file(file_path);
    if (file.open(QIODevice::WriteOnly) == false)
    {
        qDebug() << "无法打开文件：" << file_path;
        return;
    }

    file.write(QJsonDocument(json_object).toJson());
    file.close();
}

bool deleteFile(const QString& path)
{
    QFileInfo info(path);
    if (info.isFile())
    {
        return QFile::remove(path);
    }
    else if (info.isDir())
    {
        return QDir(path).removeRecursively();
    }
    return false;
}

QString copyFile(const QString& path, const QString& target_path, bool is_cover)
{
    QFileInfo is_info(path);
    QFileInfo target_info(target_path);
    if (is_info.isFile() == false)
        return QString();

    QString target_file_path;
    if (target_info.isFile())
    {
        target_file_path = target_path;
    }
    else if (target_info.isDir())
    {
        QDir dir(target_path);
        target_file_path = dir.filePath(is_info.fileName());
    }
    else
        return QString();

    if (is_cover && QFile::exists(target_file_path))
    {
        QFile::remove(target_file_path);
    }

    if (QFile::copy(path, target_file_path) == false)
    {
        return QString();
    }

    return target_file_path;
}

QList<QString> copyFolder(const QString& path, const QString& target_path, bool is_cover, bool is_while)
{
    QFileInfo is_info(path);
    if (is_info.isFile())
    {
        return { copyFile(path, target_path, is_cover) };
    }
    else if (is_info.isDir() == false)
    {
        return {};
    }
    else if (QFileInfo(target_path).isDir() == false)
    {
        return {};
    }

    QDir sourceDir(path);
    QDir targetDir(target_path + '/' + is_info.baseName());

    if (targetDir.exists() == false)
    { // 如果目标目录不存在，则进行创建
        if (targetDir.mkdir(targetDir.absolutePath()) == false)
            return QList<QString>();
    }

    QList<QString> target_list;
    QFileInfoList fileInfoList = sourceDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    for (QFileInfo& fileInfo : fileInfoList)
    {
        if (fileInfo.isDir() && is_while)
        { // 当为目录时，递归的进行copy
            QList<QString> list = copyFolder(fileInfo.path(), targetDir.path(), is_cover, is_while);
            if (list.isEmpty())
                continue;

            target_list += list;
            continue;
        }

        // 当允许覆盖操作时，将旧文件进行删除操作
        QString file_name = fileInfo.fileName();
        if (is_cover && targetDir.exists(file_name))
        {
            targetDir.remove(file_name);
        }

        // 进行文件拷贝
        if (QFile::copy(fileInfo.filePath(), targetDir.filePath(file_name)) == false)
        {
            qDebug() << "copyFolder copy failed " << fileInfo.filePath() << targetDir.filePath(file_name);
            continue;
        }
        target_list.append(fileInfo.filePath());
    }
    return target_list;
}

QString moveFile(const QString& path, const QString& target_path, bool is_cover)
{
    QFileInfo is_info(path);
    QFileInfo target_info(target_path);
    if (is_info.isFile() == false)
        return {};

    QString target_file_path;
    if (target_info.isFile())
    {
        target_file_path = target_path;
    }
    else if (target_info.isDir())
    {
        QDir dir(target_path);
        target_file_path = dir.filePath(is_info.fileName());
    }
    else
    {
        return {};
    }

    QString trash_path;                 // 回收站里返回的路径
    if (is_cover && QFile::exists(target_file_path)) {
        if (QFile::moveToTrash(target_file_path, &trash_path) == false)
            return {};
        // if (QFile::remove(target_file_path) == false)
        //     return {};
    }

    if (QFile::rename(path, target_file_path) == false) {
        QFile::rename(trash_path, target_file_path);
        return {};
    }

    return target_file_path;
}

QList<QString> moveFolder(const QString& path, const QString& target_path, bool is_cover, bool is_while)
{
    QFileInfo is_info(path);
    if (is_info.isFile())
    {
        return { moveFile(path, target_path, is_cover) };
    }
    else if (is_info.isDir() == false)
    {
        return {};
    }
    else if (QFileInfo(target_path).isDir() == false)
    {
        return {};
    }

    QDir sourceDir(path);
    QDir targetDir(target_path + '/' + is_info.baseName());

    if (targetDir.exists() == false)
    { // 如果目标目录不存在，则进行创建
        if (targetDir.mkdir(targetDir.absolutePath()) == false)
            return QList<QString>();
    }

    QList<QString> target_list;
    QFileInfoList fileInfoList = sourceDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    for (QFileInfo& fileInfo : fileInfoList)
    {
        if (fileInfo.isDir() && is_while)
        { // 当为目录时，递归的进行copy
            QList<QString> list = moveFolder(fileInfo.path(), targetDir.path(), is_cover, is_while);
            if (list.isEmpty())
                continue;

            target_list += list;
            continue;
        }

        // 当允许覆盖操作时，将旧文件进行删除操作
        QString file_name = fileInfo.fileName();
        if (is_cover && targetDir.exists(file_name))
        {
            targetDir.remove(file_name);
        }

        if (QFile::rename(fileInfo.filePath(), targetDir.filePath(file_name)) == false)
        {
            qDebug() << "moveFolder rename failed " << fileInfo.filePath() << targetDir.filePath(file_name);
            continue;
        }

        target_list.append(fileInfo.filePath());
    }
    sourceDir.removeRecursively();
    return target_list;
}

int KMP_String(const QString& data, const QString& pattern)
{
    auto getLPS_Array = [](const QString& pattern) -> QVector<int>
        {
            int m = pattern.size();
            QVector<int> lps(m);
            int length = 0;
            lps[0] = 0;

            int i = 1;
            while (i < m)
            {
                if (pattern[i] == pattern[length])
                {
                    length++;
                    lps[i] = length;
                    i++;
                }
                else
                {
                    if (length != 0)
                    {
                        length = lps[length - 1];
                    }
                    else
                    {
                        lps[i] = 0;
                        i++;
                    }
                }
            }
            return lps;
        };

    int n = data.size();
    int m = pattern.size();
    QVector<int> lps = getLPS_Array(pattern);

    int i = 0, j = 0;
    while (i < n)
    {
        if (pattern[j] == data[i])
        {
            i++;
            j++;
        }

        if (j == m)
        {
            return i - j;
        }
        else if (i < n && pattern[j] != data[i])
        {
            if (j != 0)
            {
                j = lps[j - 1];
            }
            else
            {
                i++;
            }
        }
    }
    return -1;
}

QVariant jsonFirstData(const QJsonValue& data)
{
    if (data.isArray())
        return data.toArray().at(0).toVariant();
    return data.toVariant();
}

QList<QVariant> jsonListData(const QJsonValue& data)
{
    if (data.isNull() || data.isUndefined())
        return {};
    if (data.isArray())
        return data.toArray().toVariantList();
    return { data.toVariant() };
}
