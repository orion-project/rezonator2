#ifndef ELEMENT_PROP_DLG_H
#define ELEMENT_PROP_DLG_H

#include "RezonatorDialog.h"

#include "../core/Element.h"
#include "../core/Parameters.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLineEdit;
class QStackedWidget;
class QTabWidget;
class QToolButton;
QT_END_NAMESPACE

class Element;
class ParamsEditor;
class ParamsEditorAbcd;

class ElementPropsDialog : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit ElementPropsDialog(Element *elem, QWidget *parent = nullptr);
    ~ElementPropsDialog() override;

    void populate();

    static bool editElement(class Element *elem, QWidget *parent = nullptr);

public slots:
    void collect() override;
    void reject() override;

protected:
    void showEvent(QShowEvent*) override;
    QSize prefferedSize() const override { return QSize(300, 400); }

    virtual QString verifyParams() const { return QString(); }

    // inherited from RezonatorDialog
    QString helpTopic() const override;

private:
    Element *_element;
    QTabWidget *_tabs;
    QStackedWidget *_pageParams;
    ParamsEditor *_editorParams;
    QLineEdit *_editorLabel, *_editorTitle;
    QCheckBox *_layoutShowLabel;
    QCheckBox *_layoutDrawAlt;
    QCheckBox *_elemDisabled;
    QToolButton *_butParamsMenu;
    Z::Parameters _newParams, _removedParams, _redimedParams;
    QMap<Z::Parameter*, Z::Parameter> _backupParams;
    QAction *_actnEditParam, *_actnRemoveParam;
    std::shared_ptr<ElementEventsLocker> _eventsLocker;
    std::shared_ptr<ElementMatrixLocker> _matrixLocker;

    QWidget* initPageParams();
    QWidget* initPageOptions();
    QWidget* initPageOutline();
    
    void createCustomParam();
    void editCustomParam();
    void removeCustomParam();
    void updatePageParams();
    void updateParamActions();
    
    Z::Parameters existedParams() const;
};

//------------------------------------------------------------------------------

class ElementPropsDialog_None : public ElementPropsDialog
{
    Q_OBJECT

public:
    explicit ElementPropsDialog_None(Element *elem, QWidget *parent = nullptr);
};

#endif // ELEMENT_PROP_DLG_H
