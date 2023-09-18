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
    Q_INVOKABLE State validate(QString& input, int& pos) const override;
    Q_INVOKABLE void allowPastCurrentDate(bool allow);
    Q_INVOKABLE void setDateFormat(const QString& dateFormat);
    Q_INVOKABLE void setMinDate(const QDateTime& date);
    Q_INVOKABLE void setMaxDate(const QDateTime &date);
private:
    QString m_format{};
    bool m_allow_past_current_date{};
    QDateTime m_min_date{};
    QDateTime m_max_date{};
};
}
