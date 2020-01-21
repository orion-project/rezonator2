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
    ElementsCatalogDialog(QWidget* parent = nullptr);
    ~ElementsCatalogDialog() override;

    Element* selection() const;

    /// Opens a dialog to select a sample for element creation.
    /// The sample can be one of the predefined elements stored in the Elements Catalog,
    /// and in this case the result pointer is persistent and not need to be freed.
    /// The sample can be one of the elements stored in the Custom Elements Library,
    /// then the result pointer should be freed manually after usage to prevent memory leaks.
    static Element* chooseElementSample();

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
    Schema *_library = nullptr;

    void categorySelected(int index);
    void updatePreview(Element *elem);
};

#endif // ELEMENT_SELECTOR_H
