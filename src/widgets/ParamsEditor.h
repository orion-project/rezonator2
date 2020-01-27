#ifndef PARAMS_EDITOR_H
#define PARAMS_EDITOR_H

#include <QGroupBox>

#include "../core/Parameters.h"

QT_BEGIN_NAMESPACE
class QBoxLayout;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {
    class InfoPanel;
    class ValueEdit;
}}

class ParamEditor;
class ValueEditorTS;
class ValuesEditorTS;

class ParamsEditor : public QWidget
{
    Q_OBJECT

public:
    struct Options
    {
        /// Target editing parameters.
        Z::Parameters *params;

        /// Global parameters to which tagets can be linked.
        Z::Parameters *globalParams = nullptr;

        /// Container for links to global parameters.
        Z::ParamLinks *paramLinks = nullptr;

        /// Show only parameters passing this filter.
        QSharedPointer<Z::ParameterFilter> filter;

        /// If this list is not empty, the "Menu" button is added to each editor.
        QList<QAction*> menuButtonActions;

        /// If the function is provided, it is called when each parameter editor is created.
        /// It should return an addtional control to be inserted into each parmeter editor.
        std::function<QWidget*(Z::Parameter*)> makeAuxControl;

        Options(Z::Parameters *p) : params(p) {}
    };

public:
    explicit ParamsEditor(Options opts, QWidget *parent = nullptr);

    void populateValues();

    void addEditor(Z::Parameter* param);
    void removeEditor(Z::Parameter* param);

    void focus();
    void focus(Z::Parameter *param);

    QString verify() const;

signals:
    void paramChanged(Z::Parameter* param, Z::Value value);

public slots:
    void applyValues();

private:
    Options _options;
    Z::Parameters* _params;
    QList<ParamEditor*> _editors;
    Ori::Widgets::InfoPanel* _infoPanel;
    QBoxLayout* _paramsLayout;

    void adjustEditors();
    void paramValueEdited(double value);
    void paramUnitChanged(Z::Unit unit);

private slots:
    void paramFocused();
    void focusNextParam();
    void focusPrevParam();
};

//------------------------------------------------------------------------------

class ParamsEditorTS : public QWidget
{
    Q_OBJECT

public:
    explicit ParamsEditorTS(Z::ParametersTS *params, QWidget *parent = nullptr);

    void adjustSymbolsWidth();

    void collect();

private:
    Z::ParametersTS *_params;
    ValuesEditorTS *_valuesEditor;
    QMap<Z::ParameterTS*, ValueEditorTS*> _editorsMap;
};

//------------------------------------------------------------------------------

class ParamsEditorAbcd : public QGroupBox
{
    Q_OBJECT

public:
    explicit ParamsEditorAbcd(const QString& title, const Z::Parameters& params);
    void apply();
    void populate();
    void focus();

private:
    Z::Parameters _params;
    QVector<Ori::Widgets::ValueEdit*> _editors;
};

#endif // PARAMS_EDITOR_H
