#ifndef PROGRESSSLIDER_H
#define PROGRESSSLIDER_H

#include "QAbstractSlider"
#include "QSlider"

class QProgressSlider : public QSlider
{
    Q_OBJECT

public:
    QProgressSlider();

    float value();

    virtual void setSingleStep(float value);

public slots:

    virtual void setValue(float value);

signals:
    virtual void valueChanged(float value);


protected:
    float m_value=0.0f;
    float singleStep=1.0f;
};

#endif // PROGRESSSLIDER_H
