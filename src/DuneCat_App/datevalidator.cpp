
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
    if (dt.isNull()) // If null, the input cannot be parsed
        return QValidator::Invalid;
    if(!m_max_date.isNull() && dt > m_max_date)
        return QValidator::Invalid;
    if(!m_min_date.isNull() && dt < m_min_date)
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
    m_format = dateFormat;
}

void DateValidator::setMinDate(const QDateTime &date)
{
    m_min_date = date;
}

void DateValidator::setMaxDate(const QDateTime &date)
{
    m_max_date = date;
}
}
