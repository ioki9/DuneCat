
#include "intvalidator.h"
namespace DuneCat
{
IntValidator::IntValidator(QObject *parent)
    : QObject{parent}
{

}

bool IntValidator::validate(const QString &input) const
{
    qDebug()<<"input:"<<input<<". bottom:"<<m_bottom<<". top:"<<m_top;
    int val {input.toInt()};
    return val < m_bottom || val > m_top;
}

void IntValidator::setBottomTop(int bottom, int top)
{
    m_bottom = bottom;
    m_top = top;
}

void IntValidator::setTop(int top)
{
    setBottomTop(m_bottom,top);
}

int IntValidator::getTop() const
{
    return m_top;
}

void IntValidator::setBottom(int bottom)
{
    setBottomTop(bottom,m_top);
}

int IntValidator::getBottom() const
{
    return m_bottom;
}
}
