#ifndef NE_MEDIADATA_MODEL_H
#define NE_MEDIADATA_MODEL_H

#include <NE_Data_Model.h>

class NE_MediaData_Model : public NE_Data_Model
{
    Q_OBJECT
    QML_ELEMENT
public:
    enum DataType
    {
        NE_AudioStream = NE_User + 1,
        NE_VideoStream,
        NE_ImageStream,
        NE_LyricsStream,
        NE_MusicMetaData
    };
    Q_ENUM(DataType)
    explicit NE_MediaData_Model(QObject *parent = nullptr);
};

#endif // NE_MEDIADATA_MODEL_H
