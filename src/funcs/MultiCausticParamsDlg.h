#ifndef MULTI_CAUSTIC_PARAMS_DLG_H
#define MULTI_CAUSTIC_PARAMS_DLG_H

#include "../core/Variable.h"
#include "../windows/RezonatorDialog.h"

class MultiElementSelectorWidget;
class PointsRangeEditor;

QT_BEGIN_NAMESPACE
class QCheckBox;
QT_END_NAMESPACE

/**
    The function arguments dialog that can choose several of range elements
    and set the number of points for plotting inside each of selected elements.
*/
class MultiCausticParamsDlg : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit MultiCausticParamsDlg(Schema*, const QVector<Z::Variable>&, const QString &helpTopic);

    const QVector<Z::Variable>& result() const { return _result; }

protected:
    QString helpTopic() const override { return _helpTopic; }

protected slots:
    void collect() override;

private:
    QVector<Z::Variable> _result;
    MultiElementSelectorWidget *_elemsSelector;
    PointsRangeEditor* _rangeEditor;
    QMap<Element*, Z::VariableRange> _elemRanges;
    QCheckBox *_sameSettings;
    QString _helpTopic;

    void populate(const QVector<Z::Variable>& vars);

private:
    void currentElementChanged(Element *current, Element *previous);
    void saveEditedRange(Element *elem);
};

#endif // MULTI_CAUSTIC_PARAMS_DLG_H
