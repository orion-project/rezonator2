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
        const Z::Parameters *params;

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

        /// Wether to show a panel displaying the parameter info.
        bool showInfoPanel = true;

        /// When true, the parameter change is applied immediately after the parameters was edited.
        /// When false, the client has to call ParamsEditor::applyValues() when needed.
        enum {ApplyManual, ApplyInstant, ApplyEnter} applyMode = ApplyManual;

        /// An additional control displaying at the bottom of all editors just above the info panel.
        QWidget* auxControl = nullptr;

        /// Should editor take ownership on the parameters.
        /// If yes, the parameters are deleted when the editor gets deleted.
        bool ownParams = false;

        /// When each editor's `Apply` is called, check if parameter value is really changed.
        /// Used to avoid unnecessary `parameterChanged` events.
        bool checkChanges = false;

        Options(const Z::Parameters *p = nullptr) : params(p) {}
    };

public:
    explicit ParamsEditor(const Options opts, QWidget *parent = nullptr);

    void removeEditors();
    void populateEditors();
    void populateValues();

    ParamEditor* addEditor(Z::Parameter* param, const QVector<Z::Unit>& units = {});
    void removeEditor(Z::Parameter* param);
    void populateEditor(Z::Parameter* param);
    void moveEditorUp(Z::Parameter* param);
    void moveEditorDown(Z::Parameter* param);

    void focus();
    void focus(Z::Parameter *param);

    QString verify() const;

    const QList<ParamEditor*>& editors() const { return _editors; }

signals:
    void paramChanged(Z::Parameter* param, Z::Value value);

public slots:
    void applyValues();

private:
    const Options _options;
    QList<ParamEditor*> _editors;
    Ori::Widgets::InfoPanel* _infoPanel = nullptr;
    QBoxLayout* _paramsLayout;
    void adjustEditors();
    void paramValueEdited(double value);
    void paramUnitChanged(Z::Unit unit);
    void paramEnterPressed();
    void paramFocused();
    void paramUnfocused();
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
