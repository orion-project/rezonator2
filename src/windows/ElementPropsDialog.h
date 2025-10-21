#ifndef ELEMENT_PROP_DLG_H
#define ELEMENT_PROP_DLG_H

#include "RezonatorDialog.h"

#include "../core/Parameters.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLineEdit;
class QStackedWidget;
class QTabWidget;
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
    QPushButton *_butCreateParam;
    QList<Z::Parameter*> _newParams;

    QWidget* initPageParams();
    QWidget* initPageOptions();
    QWidget* initPageOutline();
    
    void createCustomParam();
};

//------------------------------------------------------------------------------

class ElementPropsDialog_None : public ElementPropsDialog
{
    Q_OBJECT

public:
    explicit ElementPropsDialog_None(Element *elem, QWidget *parent = nullptr);
};

#endif // ELEMENT_PROP_DLG_H
