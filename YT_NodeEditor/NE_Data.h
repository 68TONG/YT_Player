#ifndef NE_DATA_H
#define NE_DATA_H

#include <NE_Data_Model.h>

class NE_Data : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit NE_Data(QQuickItem *parent = nullptr);
    ~NE_Data();

    Q_INVOKABLE void connectData(const bool IO_TYPE, NE_Data *data);
    Q_INVOKABLE void disconnectData(const bool IO_TYPE, NE_Data *data);
    Q_INVOKABLE bool isconnectData(NE_Data *data) const;
    
    void sharedDataChanged();

public:
    Q_PROPERTY(QVariant dataNE_ MEMBER dataNE_ NOTIFY dataNE_Changed)
    QVariant dataNE_;
    Q_SIGNAL void dataNE_Changed();

    Q_PROPERTY(QVariant dataNE_Default MEMBER dataNE_Default NOTIFY dataNE_DefaultChanged)
    QVariant dataNE_Default;
    Q_SIGNAL void dataNE_DefaultChanged();

    Q_PROPERTY(bool dataNE_Shared MEMBER dataNE_Shared NOTIFY dataNE_SharedChanged)
    bool dataNE_Shared = false;
    Q_SIGNAL void dataNE_SharedChanged();

    Q_PROPERTY(int typeNE_ MEMBER typeNE_ NOTIFY typeNE_Changed)
    int typeNE_ = NE_Data_Model::NE_Not;
    Q_SIGNAL void typeNE_Changed();

    Q_PROPERTY(NE_Data_Model *dataModelNE_ MEMBER dataModelNE_ NOTIFY dataModelNE_Changed)
    NE_Data_Model *dataModelNE_ = new NE_Data_Model(this);
    Q_SIGNAL void dataModelNE_Changed();

    Q_PROPERTY(QQuickItem *viewNE_ MEMBER viewNE_ NOTIFY viewNE_Changed)
    QQuickItem *viewNE_ = nullptr;
    Q_SIGNAL void viewNE_Changed();

    Q_PROPERTY(QPoint inputPos MEMBER inputPos NOTIFY inputPosChanged)
    QPoint inputPos = {};
    Q_SIGNAL void inputPosChanged();

    Q_PROPERTY(QPoint outputPos MEMBER outputPos NOTIFY outputPosChanged)
    QPoint outputPos = {};
    Q_SIGNAL void outputPosChanged();

    Q_PROPERTY(QQuickItem *inputNode MEMBER inputNode NOTIFY inputNodeChanged)
    QQuickItem *inputNode = nullptr;
    Q_SIGNAL void inputNodeChanged();

    Q_PROPERTY(QQuickItem *outputNode MEMBER outputNode NOTIFY outputNodeChanged)
    QQuickItem *outputNode = nullptr;
    Q_SIGNAL void outputNodeChanged();

    Q_PROPERTY(NE_Data *inputData READ getInputData NOTIFY inputDataChanged)
    NE_Data *inputData = nullptr;
    NE_Data *getInputData() const { return inputData; }
    Q_SIGNAL void inputDataChanged();

    Q_PROPERTY(QSet<NE_Data *> outputData READ getOutputData NOTIFY outputDataChanged)
    QSet<NE_Data *> outputData = {};
    QSet<NE_Data *> getOutputData() const { return outputData; }
    Q_SIGNAL void outputDataChanged();

    // Q_PROPERTY(NE_Node *inputNode MEMBER inputNode REQUIRED)
    // NE_Node *inputNode = nullptr;
    // Q_PROPERTY(NE_Node *outputNode MEMBER outputNode REQUIRED)
    // NE_Node *outputNode = nullptr;
protected:
    void Emit_NE_RectChanged();
    Q_SIGNAL void _NE_RectChanged();
    // void _NE_ParentChanged();
};

#endif // NE_DATA_H
