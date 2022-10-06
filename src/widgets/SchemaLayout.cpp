#include "SchemaLayout.h"

#include "../Appearance.h"
#include "../AppSettings.h"
#include "../core/ElementFormula.h"
#include "../funcs/FormatInfo.h"
#include "../io/Clipboard.h"

#include <QClipboard>
#include <QContextMenuEvent>
#include <QMenu>

namespace ElementLayoutProps {

QBrush getGlassBrush()
{
    static QBrush b = QBrush(QPixmap(":/misc/glass_pattern"));
    return b;
}

QPen getGlassPen()
{
    static QPen p = QPen(Qt::black, 1.5);
    return p;
}

QBrush getMirrorBrush()
{
    static QBrush b = QBrush(Qt::black, Qt::BDiagPattern);
    return b;
}

QPen getMirrorPen()
{
    static QPen p = QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap);
    return p;
}

QPen getPlanePen()
{
    static QPen p = QPen(Qt::black, 1, Qt::DashLine);
    return p;
}

QBrush getGrinBrush(double sizeF)
{
    int size = int(sizeF);
    static QMap<int, QBrush> brushes;
    if (!brushes.contains(size))
    {
        QLinearGradient g(0, -size, 0, size);
        g.setColorAt(0, Qt::white);
        g.setColorAt(0.5, Qt::gray);
        g.setColorAt(1, Qt::white);
        brushes[size] = QBrush(g);
    }
    return brushes[size];
}

const QFont& getMarkTSFont()
{
    static QFont f = QFont("Arial", 8, QFont::Bold);
    return f;
}

const QFont& getLabelFont()
{
    static QFont f = Z::Gui::ElemLabelFont().get();
    return f;
}

} // namespace ElementLayoutProps

using namespace ElementLayoutProps;

//------------------------------------------------------------------------------
//                             ElementLayout
//------------------------------------------------------------------------------

ElementLayout::ElementLayout(Element* elem): QGraphicsItem(), _element(elem)
{
}

ElementLayout::~ElementLayout()
{
}

void ElementLayout::makeElemToolTip()
{
    if (!_element) return;
    auto schema = dynamic_cast<Schema*>(_element->owner());
    if (!schema) return;

    Z::Format::FormatElemParams f;
    f.schema = schema;
    QString paramStr = f.format(_element);
    paramStr.replace(QStringLiteral(", "), QStringLiteral("<br>"));
    setToolTip(QStringLiteral("#%1 <b>%2</b><br><i>(%3)</i><br>%4")
               .arg(schema->indexOf(_element)+1)
               .arg(_element->label(), _element->typeName(), paramStr));
}

QRectF ElementLayout::boundingRect() const
{
    return QRectF(-HW, -HH, 2*HW, 2*HH);
}

void ElementLayout::setSlope(double elementAngle)
{
    if (elementAngle > 0)
        _slope = SlopePlus;
    else if (elementAngle < 0)
        _slope = SlopeMinus;
    else
        _slope = SlopeNone;
}

void ElementLayout::slopePainter(QPainter *painter)
{
    if (_slope != SlopeNone)
    {
        qreal angle = (_slope == SlopePlus)? _slopeAngle: -_slopeAngle;
        QTransform t = painter->transform();
        t.rotate(angle);
        painter->setTransform(t);

        t.rotate(-2*angle);
        QBrush b = painter->brush();
        b.setTransform(t);
        painter->setBrush(b);
    }
}

//------------------------------------------------------------------------------
namespace OpticalAxisLayout {
    DECLARE_ELEMENT_LAYOUT_BEGIN
    DECLARE_ELEMENT_LAYOUT_END

    ELEMENT_LAYOUT_INIT {
    }

    ELEMENT_LAYOUT_PAINT {
        painter->drawLine(QLineF(-HW, 0, HW, 0));
    }
}

//------------------------------------------------------------------------------
//                               SchemaLayout
//------------------------------------------------------------------------------

SchemaLayout::SchemaLayout(Schema *schema, QWidget* parent) : QGraphicsView(parent), _schema(schema)
{
    _axis = new OpticalAxisLayout::Layout(nullptr);
    _axis->setZValue(1000);
    _scene.addItem(_axis);

    setRenderHint(QPainter::Antialiasing, true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setScene(&_scene);
}

SchemaLayout::~SchemaLayout()
{
    clear();
}

void SchemaLayout::populate()
{
    setUpdatesEnabled(false);

    clear();

    auto elems = _schema->elements();
    for (int i = 0; i < elems.size(); i++) {
        Element *elem = elems.at(i);
        if (elem->disabled()) continue;
        auto layout = ElementLayoutFactory::make(elem);
        if (layout) {
            layout->makeElemToolTip();
            layout->init();
            addElement(layout);
        }
    }

    // Calculate axis length and position.
    // The first element is at zero position.
    qreal fullW = 0;
    qreal firstHW = -1;
    foreach (ElementLayout *elem, _elements) {
        fullW += 2*elem->halfW();
        if (firstHW < 0)
            firstHW = elem->halfW();
    }
    const qreal axisMargin = 10;
    fullW += axisMargin + axisMargin;
    _axis->setHalfSize(fullW/2, 5);
    _axis->setX(fullW/2 - firstHW - axisMargin);

    auto r = _scene.itemsBoundingRect();
    r.adjust(-10, -10, 10, 10);
    _scene.setSceneRect(r);
    centerView(r);

    setUpdatesEnabled(true);
}

void SchemaLayout::addElement(ElementLayout *elem)
{
    if (!_elements.isEmpty()) {
        ElementLayout *last = _elements.last();
        elem->setPos(last->x() + last->halfW() + elem->halfW(), 0);
    }
    else elem->setPos(0, 0);

    _elements.append(elem);
    _scene.addItem(elem);

    // Add element label
    if (elem->element()->layoutOptions.showLabel) {
        QGraphicsTextItem *label = _scene.addText(elem->element()->label());
        label->setZValue(1000 + _elements.count());
        label->setFont(getLabelFont());
        label->setToolTip(elem->toolTip());
        // Try to position new label avoiding overlapping with previous labels
        QRectF r = label->boundingRect();
        qreal labelX = elem->x() - r.width() / 2.0;
        qreal labelY = elem->y() - elem->halfH() - r.height();
        qreal minY = labelY;
        for (int prevIndex = _elements.size()-2; prevIndex >= 0; prevIndex--) {
            auto elemLayout = _elements.at(prevIndex);
            if (not elemLayout->element()->layoutOptions.showLabel) continue;
            auto prevLabel = _elemLabels[elemLayout];
            auto prevRect = prevLabel->boundingRect();
            if (labelX <= prevLabel->x() + prevRect.width() &&
                labelY <= prevLabel->y() && labelY > prevLabel->y() - prevRect.height())
                labelY = minY - prevRect.height()*0.75;
            else minY = qMin(minY, prevLabel->y());
        }
        label->setX(labelX);
        label->setY(labelY);
        _elemLabels.insert(elem, label);
    }
}

void SchemaLayout::clear()
{
    _scene.removeItem(_axis);
    _scene.clear();
    _elemLabels.clear();
    _scene.addItem(_axis);
    _elements.clear();
}

void SchemaLayout::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    //centerView(QRectF());
}

void SchemaLayout::centerView(const QRectF& rect)
{
    QRectF r = rect.isEmpty() ? _scene.itemsBoundingRect(): rect;
    centerOn(r.center());
}

void SchemaLayout::contextMenuEvent(QContextMenuEvent *event)
{
    if (!_menu) _menu = createContextMenu();
    _menu->popup(this->mapToGlobal(event->pos()));
}

QMenu* SchemaLayout::createContextMenu()
{
    auto menu = new QMenu(this);
    menu->addAction(QIcon(":/toolbar/copy_img"), tr("Copy Image"), this, &SchemaLayout::copyImage);
    return menu;
}

void SchemaLayout::copyImage()
{
    Z::IO::Clipboard::setImage(this, AppSettings::instance().layoutExportTransparent);
}

//------------------------------------------------------------------------------
//                             ElementLayoutFactory
//------------------------------------------------------------------------------

#include "SchemaLayoutElems.h"
#include "SchemaLayoutLens.h"
#include "SchemaLayoutCrystal.h"
#include "SchemaLayoutAxicon.h"
#include "SchemaLayoutIfaces.h"

namespace ElementLayoutFactory {

static QMap<QString, std::function<ElementLayout*(Element*)>> __factoryMethods;

template <class TElement, class TLayout> void registerLayout() {
    TElement tmp;
    __factoryMethods.insert(tmp.type(), [](Element*e) { return new TLayout(e); });
}

ElementLayout* make(Element *elem) {
    if (__factoryMethods.empty()) {
        registerLayout<ElemAxiconLens, ElemAxiconLensLayout::Layout>();
        registerLayout<ElemAxiconMirror, ElemAxiconMirrorLayout::Layout>();
        registerLayout<ElemBrewsterCrystal, ElemBrewsterCrystalLayout::Layout>();
        registerLayout<ElemBrewsterInterface, ElemBrewsterInterfaceLayout::Layout>();
        registerLayout<ElemBrewsterPlate, ElemBrewsterPlateLayout::Layout>();
        registerLayout<ElemCurveMirror, ElemCurveMirrorLayout::Layout>();
        registerLayout<ElemCylinderLensS, ElemCylinderLensSLayout::Layout>();
        registerLayout<ElemCylinderLensT, ElemCylinderLensTLayout::Layout>();
        registerLayout<ElemEmptyRange, ElemEmptyRangeLayout::Layout>();
        registerLayout<ElemFlatMirror, ElemFlatMirrorLayout::Layout>();
        registerLayout<ElemFormula, ElemMatrixLayout::Layout>();
        registerLayout<ElemGaussAperture, ElemGaussApertureLayout::Layout>();
        registerLayout<ElemGaussApertureLens, ElemGaussApertureLensLayout::Layout>();
        registerLayout<ElemGaussDuctSlab, ElemGrinLensLayout::Layout>();
        registerLayout<ElemGaussDuctMedium, ElemGrinMediumLayout::Layout>();
        registerLayout<ElemGrinLens, ElemGrinLensLayout::Layout>();
        registerLayout<ElemGrinMedium, ElemGrinMediumLayout::Layout>();
        registerLayout<ElemMatrix, ElemMatrixLayout::Layout>();
        registerLayout<ElemMatrix1, ElemMatrixLayout::Layout>();
        registerLayout<ElemMediumRange, ElemMediumRangeLayout::Layout>();
        registerLayout<ElemNormalInterface, ElemNormalInterfaceLayout::Layout>();
        registerLayout<ElemPlate, ElemPlateLayout::Layout>();
        registerLayout<ElemPoint, ElemPointLayout::Layout>();
        registerLayout<ElemSphericalInterface, ElemSphericalInterfaceLayout::Layout>();
        registerLayout<ElemThermoLens, ElemGrinLensLayout::Layout>();
        registerLayout<ElemThermoMedium, ElemGrinMediumLayout::Layout>();
        registerLayout<ElemThickLens, ElemThickLensLayout::Layout>();
        registerLayout<ElemThinLens, ElemThinLensLayout::Layout>();
        registerLayout<ElemTiltedCrystal, ElemTiltedCrystalLayout::Layout>();
        registerLayout<ElemTiltedInterface, ElemTiltedInterfaceLayout::Layout>();
        registerLayout<ElemTiltedPlate, ElemTiltedPlateLayout::Layout>();
    }
    if (!__factoryMethods.contains(elem->type()))
        return nullptr;
    return __factoryMethods[elem->type()](elem);
}

} // namespace ElementLayoutFactory
