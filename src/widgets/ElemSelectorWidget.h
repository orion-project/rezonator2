#ifndef ELEM_SELECTOR_WIDGET_H
#define ELEM_SELECTOR_WIDGET_H

#include <QComboBox>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
QT_END_NAMESPACE

#include "WidgetResult.h"
#include "../core/Schema.h"
#include "../core/ElementFilter.h"

/**
    Combo-box widget which allows to choose one of schema elements.
*/
class ElemSelectorWidget : public QComboBox
{
    Q_OBJECT

public:
    explicit ElemSelectorWidget(Schema* schema, ElementFilter* filter = nullptr);

    Element* selectedElement() const;
    void setSelectedElement(Element *elem);

    const Elements& elements() const { return _elements; }

    WidgetResult verify();

private:
    Elements _elements;
    ElementFilter* _filter;
};


/**
    Combo-box widget which allows to choose one of parameter of element.
*/
class ParamSelectorWidget : public QComboBox
{
    Q_OBJECT

public:
    explicit ParamSelectorWidget(Z::ParameterFilter* filter = nullptr);

    void populate(Element*);

    Z::Parameter* selectedParameter() const;
    void setSelectedParameter(Z::Parameter* param);

    const Z::Parameters& parameters() const { return _parameters; }

    WidgetResult verify();

private:
    Z::Parameters _parameters;
    Z::ParameterFilter* _filter;
};


/**
    Layout combinig \ref ElemSelectorWidget and \ref ParamSelectorWidget together.
*/
class ElemAndParamSelector : public QGridLayout
{
    Q_OBJECT

public:
    explicit ElemAndParamSelector(Schema* schema,
        ElementFilter *elemFilter = nullptr, Z::ParameterFilter* paramFilter = nullptr);

    Element* selectedElement() const { return _elemSelector->selectedElement(); }
    void setSelectedElement(Element *elem) { _elemSelector->setSelectedElement(elem); }

    Z::Parameter* selectedParameter() const { return _paramSelector->selectedParameter(); }
    void setSelectedParameter(Z::Parameter* param) { _paramSelector->setSelectedParameter(param); }

    WidgetResult verify();

    const ElemSelectorWidget* elemSelector() const { return _elemSelector; }
    const ParamSelectorWidget* paramSelector() const { return _paramSelector; }

signals:
    void selectionChanged();

private:
    ElemSelectorWidget* _elemSelector;
    ParamSelectorWidget* _paramSelector;

private slots:
    void currentElemChanged(int);
    void currentParamChanged(int);
};


/**
    List widget which allows to choose one of schema elements.
*/
class MultiElementSelectorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MultiElementSelectorWidget(Schema* schema, ElementFilter *filter = nullptr);

    Elements allElements() const;
    Elements selectedElements() const;
    Element* currentElement() const;
    void selectElement(Element*);
    void setCurrentRow(int index);

signals:
    void currentElementChanged(Element* currentElement, Element* previous);

private slots:
    void selectAllElements();
    void deselectAllElements();
    void invertElementsSelection();

private:
    QListWidget* _elemsSelector;
    QMap<Element*, QListWidgetItem*> _itemsMap;

    void populate(Schema* schema, ElementFilter *filter);
    void currentItemChanged(QListWidgetItem *currentElement, QListWidgetItem *previous);
    void invertCheckState(QListWidgetItem *item);
    Element* element(QListWidgetItem *item) const;
};

#endif // ELEM_SELECTOR_WIDGET_H
