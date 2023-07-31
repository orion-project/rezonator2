#ifndef ELEMENT_PROP_DLG_H
#define ELEMENT_PROP_DLG_H

#include "RezonatorDialog.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLineEdit;
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

    virtual void populateParams() {}
    virtual void collectParams() {}
    virtual QString verifyParams() const { return QString(); }

    QString helpTopic() const override;

    void setPageParams(QWidget* pageParams);

private:
    Element *_element;
    QTabWidget *_tabs;
    QLineEdit *_editorLabel, *_editorTitle;
    QCheckBox *_layoutShowLabel;
    QCheckBox *_layoutDrawAlt;
    QCheckBox *_elemDisabled;

    QWidget* initPageOptions();
    QWidget* initPageOutline();
};

//------------------------------------------------------------------------------

class ElementPropsDialog_None : public ElementPropsDialog
{
    Q_OBJECT

public:
    explicit ElementPropsDialog_None(Element *elem, QWidget *parent = nullptr);
};

//------------------------------------------------------------------------------

class ElementPropsDialog_List : public ElementPropsDialog
{
    Q_OBJECT

public:
    explicit ElementPropsDialog_List(Element *elem, QWidget *parent = nullptr);

protected:
    void populateParams() override;
    void collectParams() override;
    virtual QString verifyParams() const override;

private:
    ParamsEditor* _editors;
};

//------------------------------------------------------------------------------

class ElementPropsDialog_Abcd : public ElementPropsDialog
{
    Q_OBJECT

public:
    explicit ElementPropsDialog_Abcd(Element *elem, QWidget *parent = nullptr);

protected:
    void populateParams() override;
    void collectParams() override;

private:
    ParamsEditorAbcd *_editorMt, *_editorMs;
};

//------------------------------------------------------------------------------

#endif // ELEMENT_PROP_DLG_H
