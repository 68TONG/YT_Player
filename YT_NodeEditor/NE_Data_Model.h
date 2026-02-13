#pragma once

#include <QObject>
#include <QQuickItem>

class NE_Data;
class NE_Data_Model : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    friend NE_Data;

public:
    enum DataType
    {
        NE_Not,
        NE_Int,
        NE_Float,
        NE_String,
        NE_Json,
        NE_User
    };
    Q_ENUM(DataType)
    explicit NE_Data_Model(QObject *parent = nullptr);

protected:
    virtual bool inputDataFunc(NE_Data *input_data, NE_Data *output_data);
    virtual bool disinputDataFunc(NE_Data *input_data, NE_Data *output_data);
};
