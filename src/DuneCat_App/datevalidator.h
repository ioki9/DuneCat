#pragma once
#include <QValidator>
#include <QDateTime>
namespace DuneCat
{
class DateValidator : public QValidator
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(DateValidator)
public:
    explicit DateValidator(QObject *parent = nullptr);
    State validate(QString& input, int& pos) const;
    Q_INVOKABLE void allowPastCurrentDate(bool allow);
    Q_INVOKABLE void setDateFormat(const QString& dateFormat);
private:
    QString m_format{};
    bool m_allow_past_current_date{};
};
}
