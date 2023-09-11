#pragma once
#include <QValidator>
namespace DuneCat
{
class IntValidator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int top READ getTop WRITE setTop NOTIFY topChanged FINAL)
    Q_PROPERTY(int bottom READ getBottom WRITE setBottom NOTIFY bottomChanged FINAL)

public:
    explicit IntValidator(QObject *parent = nullptr);
    Q_INVOKABLE bool validate(const QString& input) const;
    Q_INVOKABLE void setBottomTop(int bottom,int top);
    Q_INVOKABLE void setBottom(int bottom);
    Q_INVOKABLE void setTop(int top);
    Q_INVOKABLE int getTop();
    Q_INVOKABLE int getBottom();
signals:
    void bottomChanged();
    void topChanged();
private:
    int m_bottom{};
    int m_top{};
};
}
