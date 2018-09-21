#ifndef ELEMENT_SELECTOR_H
#define ELEMENT_SELECTOR_H

#include "RezonatorDialog.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QTabWidget;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {
    class SvgView;
}}

class Element;

class ElementsCatalogDialog : public RezonatorDialog
{
    Q_OBJECT

public:
    enum CatalogMode
    {
        CatalogMode_Selector,
        CatalogMode_View
    };

    ElementsCatalogDialog(CatalogMode mode, QWidget* parent = nullptr);
    ~ElementsCatalogDialog() override;

    QString selected() const;

protected:
    QSize prefferedSize() const override { return QSize(600, 400); }

private:
    class ElementTypesListView *elements;
    Ori::Widgets::SvgView *drawing;
    QVBoxLayout *layoutPage;
    QTabWidget *tabs;

private slots:
    void categorySelected(int index);
    void loadDrawing(const QString& elemType);
};


namespace Z {
namespace Dlgs {

bool selectElementDialog(QString &type);
Element* createElement();
void showElementsCatalog();

}}

#endif // ELEMENT_SELECTOR_H
