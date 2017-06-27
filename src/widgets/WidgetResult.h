#ifndef WIDGET_RESULT_H
#define WIDGET_RESULT_H

#include <QString>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class WidgetResult
{
public:
    static WidgetResult ok()
    {
        return WidgetResult();
    }

    static WidgetResult fail(QWidget* widget, const QString& message)
    {
        WidgetResult wr;
        wr._result = false;
        wr._widget = widget;
        wr._message = message;
        return wr;
    }

    operator bool() const { return _result; }
    bool operator !() const { return !_result; }

    void show(QWidget* parent);

private:
    WidgetResult() {}

    bool _result = true;
    QWidget* _widget = nullptr;
    QString _message = QString();

    QWidget* findFocusable(QWidget*);
};

#endif // WIDGET_RESULT_H
