
#include "datevalidator.h"
namespace DuneCat
{
DateValidator::DateValidator(QObject *parent)
    : QValidator{parent}
{
    QLocale locale{};
    m_format = locale.dateFormat(QLocale::ShortFormat);
}

QValidator::State DateValidator::validate(QString& input, int& pos) const
{

    QDateTime dt = QDateTime::fromString(input,m_format);
    qDebug()<<dt;
    if (dt.isNull()) // If null, the input cannot be parsed
        return QValidator::Invalid;

    return QValidator::Acceptable;
}

void DateValidator::allowPastCurrentDate(bool allow)
{
    m_allow_past_current_date = allow;
}
// yyyy - yeaar
// MM - month
// dd - days
void DateValidator::setDateFormat(const QString &dateFormat)
{
    qDebug()<<dateFormat;
    m_format = dateFormat;
}
}
