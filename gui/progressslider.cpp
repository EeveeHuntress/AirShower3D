#include "progressslider.h"

QProgressSlider::QProgressSlider()
{

}

float QProgressSlider::value()
{
    return m_value;
}

void QProgressSlider::setValue(float value)
{
    if(m_value!=value)
    {
        m_value=value;
        emit valueChanged(value);
    }

}

void QProgressSlider::setSingleStep(float value)
{
    singleStep=value;
}


