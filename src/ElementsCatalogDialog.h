#ifndef ELEMENT_SELECTOR_H
#define ELEMENT_SELECTOR_H

#include "RezonatorDialog.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QTabWidget;
class QStackedWidget;
class QTextBrowser;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {
    class SvgView;
}}

class Schema;
class Element;
class ElementTypesListView;

class ElementsCatalogDialog : public RezonatorDialog
{
    Q_OBJECT

public:
    enum CatalogMode
    {
        CatalogMode_Selector,
        CatalogMode_View
    };

    struct Selection
    {
        Element* elem;
        bool isCustom;
    };

    ElementsCatalogDialog(CatalogMode mode, QWidget* parent = nullptr);
    ~ElementsCatalogDialog() override;

    Selection selection() const;

protected:
    QSize prefferedSize() const override { return QSize(600, 400); }

private:
    ElementTypesListView *_elementsList;
    Ori::Widgets::SvgView *_previewSvg;
    QVBoxLayout *_pageLayout;
    QStackedWidget *_preview;
    QTabWidget *_categoryTabs;
    QTextBrowser *_previewHtml = nullptr;
    int _customElemsTab = -1;
    Schema *_customElems = nullptr;

private slots:
    void categorySelected(int index);
    void loadDrawing(Element *elem);
};


namespace Z {
namespace Dlgs {

Element* createElement();
void showElementsCatalog();

}}

#endif // ELEMENT_SELECTOR_H
