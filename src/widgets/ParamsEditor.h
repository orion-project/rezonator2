#ifndef PARAMS_EDITOR_H
#define PARAMS_EDITOR_H

#include <QGroupBox>

#include "ParamEditor.h"
#include "../core/Parameters.h"

namespace Ori {
namespace Widgets {
    class InfoPanel;
    class ValueEdit;
}}

class ParamsEditor : public QWidget
{
    Q_OBJECT

public:
    struct Options
    {
        /// Target editing parameters
        Z::Parameters *params;

        /// Global parameters to which tagets can be linked
        Z::Parameters *globalParams = nullptr;

        Options(Z::Parameters *p) : params(p) {}
    };


public:
    explicit ParamsEditor(Options opts, QWidget *parent = 0);

    void populate();

    void focus();
    void focus(Z::Parameter *param);

    QString verify() const;

public slots:
    void apply();

private:
    Z::Parameters* _params;
    QList<ParamEditor*> _editors;
    Ori::Widgets::InfoPanel* _infoPanel;

    void adjustEditors();

private slots:
    void paramFocused();
    void focusNextParam();
    void focusPrevParam();
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
