#ifndef NE_NODEDATA_H
#define NE_NODEDATA_H

#include <YT_GeneralH.h>

class NE_NodeData : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit NE_NodeData(QObject *parent = nullptr);

signals:
};

#endif // NE_NODEDATA_H
