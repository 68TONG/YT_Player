#include "NE_Data.h"

NE_Data::NE_Data(QQuickItem *parent)
    : QQuickItem{parent}
{
    this->setFlag(QQuickItem::ItemHasContents, true);

    connect(this, &NE_Data::xChanged, this, &NE_Data::Emit_NE_RectChanged);
    connect(this, &NE_Data::yChanged, this, &NE_Data::Emit_NE_RectChanged);
    connect(this, &NE_Data::widthChanged, this, &NE_Data::Emit_NE_RectChanged);
    connect(this, &NE_Data::heightChanged, this, &NE_Data::Emit_NE_RectChanged);
    // connect(this, &NE_Data::parentChanged, this, [this](QQuickItem *parent){});
}

NE_Data::~NE_Data()
{
    if (inputData)
    {
        disconnectData(false, inputData);
    }
    while (outputData.isEmpty() == false)
    {
        auto it = *outputData.begin();
        disconnectData(true, it);
    }
}

void NE_Data::connectData(const bool IO_TYPE, NE_Data *data)
{
    if (data == this)
        return;
    if (IO_TYPE)
    {
        if (data->inputData == this)
            return;
        if (data->inputData)
            data->inputData->disconnectData(true, data);

        data->inputData = this;
        outputData.insert(data);
        emit data->inputDataChanged();
        emit outputDataChanged();

        if (data->dataModelNE_ ? data->dataModelNE_->inputDataFunc(data, this) : false)
        {
            emit data->dataNE_Changed();
            if (data->dataNE_Shared)
            {
                connect(this, &NE_Data::dataNE_Changed, data, &NE_Data::sharedDataChanged);
            }
        }
    }
    else
    {
        if (inputData == data)
            return;
        if (inputData)
            inputData->disconnectData(true, this);

        inputData = data;
        data->outputData.insert(this);
        emit inputDataChanged();
        emit data->outputDataChanged();

        if (this->dataModelNE_ ? this->dataModelNE_->inputDataFunc(this, data) : false)
        {
            emit this->dataNE_Changed();
            if (this->dataNE_Shared)
            {
                connect(data, &NE_Data::dataNE_Changed, this, &NE_Data::sharedDataChanged);
            }
        }
    }
}

void NE_Data::disconnectData(const bool IO_TYPE, NE_Data *data)
{
    if (data == this)
        return;
    if (IO_TYPE)
    {
        if (data->inputData != this)
            return;

        data->inputData = nullptr;
        outputData.remove(data);
        emit data->inputDataChanged();
        emit outputDataChanged();

        if (data->dataModelNE_ ? data->dataModelNE_->disinputDataFunc(data, this) : false)
        {
            emit data->dataNE_Changed();
            if (data->dataNE_Shared)
            {
                disconnect(this, &NE_Data::dataNE_Changed, data, &NE_Data::sharedDataChanged);
            }
        }
    }
    else
    {
        if (inputData != data)
            return;

        inputData = nullptr;
        data->outputData.remove(this);
        emit inputDataChanged();
        emit data->outputDataChanged();

        if (this->dataModelNE_ ? this->dataModelNE_->inputDataFunc(this, data) : false)
        {
            emit this->dataNE_Changed();
            if (this->dataNE_Shared)
            {
                disconnect(data, &NE_Data::dataNE_Changed, this, &NE_Data::sharedDataChanged);
            }
        }
    }
}

bool NE_Data::isconnectData(NE_Data *data) const
{
    if (data == inputData)
        return true;
    if (outputData.find(data) != outputData.end())
        return true;
    return false;
}

void NE_Data::sharedDataChanged()
{
    dataModelNE_->inputDataFunc(this, this->inputData);
    emit this->dataNE_Changed();
}

void NE_Data::Emit_NE_RectChanged()
{
    emit _NE_RectChanged();
    auto children = this->findChildren<NE_Data *>();
    for (auto &&it : children)
    {
        emit it->_NE_RectChanged();
    }
}
