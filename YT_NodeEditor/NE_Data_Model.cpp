#include "NE_Data_Model.h"
#include "NE_Data.h"

NE_Data_Model::NE_Data_Model(QObject *parent)
    : QObject{parent}
{
}

bool NE_Data_Model::inputDataFunc(NE_Data *input_data, NE_Data *output_data)
{
    if (input_data->typeNE_ == output_data->typeNE_)
    {
        input_data->dataNE_ = output_data->dataNE_;
        return true;
    }

    static const QList<int> general_types = {NE_Int, NE_Float, NE_String, NE_Json};
    if (general_types.contains(input_data->typeNE_) && general_types.contains(output_data->typeNE_))
    {
        input_data->dataNE_ = output_data->dataNE_;
        return true;
    }

    return false;
}

bool NE_Data_Model::disinputDataFunc(NE_Data *input_data, NE_Data *output_data)
{
    if (input_data->dataNE_Default.isValid())
    {
        input_data->dataNE_ = input_data->dataNE_Default;
        return true;
    }

    if (input_data->typeNE_ == NE_Int)
    {
        input_data->dataNE_ = 0;
        return true;
    }

    if (input_data->typeNE_ == NE_Float)
    {
        input_data->dataNE_ = 0.0f;
        return true;
    }

    if (input_data->typeNE_ == NE_String)
    {
        input_data->dataNE_ = "";
        return true;
    }

    if (input_data->typeNE_ == NE_Json)
    {
        input_data->dataNE_ = "";
        return true;
    }

    return false;
}
