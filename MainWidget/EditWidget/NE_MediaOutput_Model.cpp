#include "NE_MediaOutput_Model.h"

NE_MediaOutput_Model::NE_MediaOutput_Model(QObject* parent)
    : NE_MediaData_Model{ parent }
{
}

void NE_MediaOutput_Model::start()
{
    const QString output_path = "./YT_PlayerOutput/" + fileName + '.' + fileType;
    int sic = media_encoder.setMediaPath(output_path.toStdString().c_str());
    if (sic < 0)
        return;

    for (auto it : dataModel)
    {
        if (it->dataNE_.isNull() || it->dataNE_.isValid() == false)
            continue;
        if (it->typeNE_ == NE_Json)
        {
            YT_Func::MediaEncoder_Write_YT_Data(media_encoder, it->dataNE_.toString());
        }
        else if (it->typeNE_ == NE_AudioStream)
        {
            media_encoder.addStream(it->dataNE_.value<AudioBuffer*>());
        }
        else if (it->typeNE_ == NE_VideoStream)
        {
            media_encoder.addStream(it->dataNE_.value<VideoBuffer*>());
        }
        else if (it->typeNE_ == NE_ImageStream)
        {
            media_encoder.addStream(it->dataNE_.value<VideoBuffer*>());
        }
        else if (it->typeNE_ == NE_LyricsStream)
        {
            media_encoder.addLyricsStream(it->dataNE_.toString().toStdString().c_str());
        }
    }

    int task_id = ThreadPool::getObject().getTaskID();
    ThreadPool::getObject().addTask(task_id, [](MediaEncoder* media_encoder)
        { media_encoder->encodeRun(); }, &media_encoder);
    ThreadPool::getObject().addTask_Release(task_id, [](MediaEncoder* media_encoder)
        { media_encoder->release(); }, &media_encoder);
}

bool NE_MediaOutput_Model::inputDataFunc(NE_Data* input_data, NE_Data* output_data)
{
    if (output_data->typeNE_ != NE_String)
        return false;

    auto info = QFileInfo(output_data->dataNE_.toString());
    if (info.isFile() == false)
        return false;

    fileName = info.completeBaseName();
    fileType = info.suffix();

    emit fileNameChanged();
    emit fileTypeChanged();
    return true;
}

void NE_MediaOutput_Model::insertOutputData(NE_Data* data)
{
    if (dataModel.indexOf(data) >= 0)
        return;
    dataModel.append(data);
    emit dataModelChanged();
}

void NE_MediaOutput_Model::removeOutputData(const int index)
{
    dataModel.remove(index);
    emit dataModelChanged();
}
