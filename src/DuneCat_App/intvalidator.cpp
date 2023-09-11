
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
    if(val < m_bottom || val > m_top)
        return false;

    return true;
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

int IntValidator::getTop()
{
    return m_top;
}

void IntValidator::setBottom(int bottom)
{
    setBottomTop(bottom,m_top);
}

int IntValidator::getBottom()
{
    return m_bottom;
}
}
