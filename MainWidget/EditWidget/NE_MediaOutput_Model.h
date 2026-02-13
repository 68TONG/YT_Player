#ifndef NE_MEDIAOUTPUT_MODEL_H
#define NE_MEDIAOUTPUT_MODEL_H

#include <YT_Info.h>
#include <YT_Func.h>
#include <YT_DataBase.h>
#include <ThreadPool.h>
#include <MediaDecoder.h>
#include <MediaEncoder.h>
#include <NE_Data.h>
#include <NE_MediaData_Model.h>

class NE_MediaOutput_Model : public NE_MediaData_Model
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit NE_MediaOutput_Model(QObject *parent = nullptr);

    Q_INVOKABLE void start();
    virtual bool inputDataFunc(NE_Data *input_data, NE_Data *output_data) override;

    Q_PROPERTY(QString fileName MEMBER fileName NOTIFY fileNameChanged)
    QString fileName;
    Q_SIGNAL void fileNameChanged();

    Q_PROPERTY(QString fileType MEMBER fileType NOTIFY fileTypeChanged)
    QString fileType;
    Q_SIGNAL void fileTypeChanged();

    Q_PROPERTY(QList<NE_Data *> dataModel READ getDataModel NOTIFY dataModelChanged)
    QList<NE_Data *> dataModel;
    QList<NE_Data *> getDataModel() const { return dataModel; }
    Q_SIGNAL void dataModelChanged();
    Q_INVOKABLE void insertOutputData(NE_Data *data);
    Q_INVOKABLE void removeOutputData(const int index);

protected:
    MediaEncoder media_encoder;
};

#endif // NE_MEDIAOUTPUT_MODEL_H
