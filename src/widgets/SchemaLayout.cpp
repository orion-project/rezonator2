#include "SchemaLayout.h"
#include "SchemaLayoutDefs.h"

#include "../app/Appearance.h"
#include "../core/Elements.h"
#include "../core/ElementFormula.h"
#include "../math/FormatInfo.h"

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

const QPen& ElementLayout::getSelectedAxisPen() const
{
    //static QPen p(QColor(140, 170, 240), 7, Qt::SolidLine, Qt::FlatCap);
    static QPen p(QColor(0, 0, 255, 75), 7, Qt::SolidLine, Qt::FlatCap);
    return p;
}

const QBrush& ElementLayout::getGlassBrush() const
{
    // Brushes made from vector icons look nice on 4k-display
    // but they look bad when rendered on the secondary low-dpi display
    // And they look the same bad when layout images is copied to cliopboard, which is more importan
    // While upscaled raster images look more or less ok in both cases
    //static QBrush b1 = QBrush(QIcon(":/misc/glass_pattern").pixmap(24));
    //static QBrush b2 = QBrush(QIcon(":/misc/glass_pattern_selected").pixmap(24));
    static QBrush b1 = QBrush(QPixmap(":/misc/glass_pattern"));
    static QBrush b2 = QBrush(QPixmap(":/misc/glass_pattern_selected"));
    return _selected ? b2 : b1;
}

const QPen &ElementLayout::getGlassPen() const
{
    static QPen p1 = QPen(Qt::black, 1.5);
    static QPen p2 = QPen(Qt::blue, 2.5);
    return _selected ? p2 : p1;
}

const QBrush& ElementLayout::getMirrorBrush() const
{
    static QBrush b1 = QBrush(Qt::black, Qt::BDiagPattern);
    static QBrush b2 = QBrush(Qt::blue, Qt::BDiagPattern);
    return _selected ? b2 : b1;
}

const QPen &ElementLayout::getMirrorPen() const
{
    static QPen p1 = QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap);
    static QPen p2 = QPen(Qt::blue, 3, Qt::SolidLine, Qt::FlatCap);
    return _selected ? p2 : p1;
}

const QPen &ElementLayout::getPlanePen() const
{
    static QPen p1 = QPen(Qt::black, 1, Qt::DashLine);
    static QPen p2 = QPen(Qt::blue, 1, Qt::DashLine);
    return _selected ? p2 : p1;
}

const QBrush &ElementLayout::getGrinBrush(double sizeF) const
{
    int size = int(sizeF);
    static QMap<int, QPair<QBrush, QBrush>> brushes;
    if (!brushes.contains(size))
    {
        QLinearGradient g1(0, -size, 0, size);
        g1.setColorAt(0, Qt::white);
        g1.setColorAt(0.5, Qt::gray);
        g1.setColorAt(1, Qt::white);
        QLinearGradient g2(0, -size, 0, size);
        g2.setColorAt(0, QColor(225, 230, 255));
        g2.setColorAt(0.5, QColor(140, 170, 240));
        g2.setColorAt(1, QColor(225, 230, 255));
        brushes[size] = { QBrush(g1), QBrush(g2) };
    }
    return _selected ? brushes[size].second : brushes[size].first;
}

const QFont& ElementLayout::getMarkTSFont() const
{
    static QFont f = QFont("Arial", 8, QFont::Bold);
    return f;
}

//------------------------------------------------------------------------------
//                            ElementLayoutOptionsView
//------------------------------------------------------------------------------

const char* ElementLayoutOptionsView::altVersionOptionTitle() const
{
    return QT_TRANSLATE_NOOP("LayoutOptions", "Draw narrow version of element");
}

//------------------------------------------------------------------------------
namespace OpticalAxisLayout {
    LAYOUT_BEGIN

    PAINT {
        painter->drawLine(QLineF(-HW, 0, HW, 0));
    }

    LAYOUT_END
}

//------------------------------------------------------------------------------
//                               SchemaLayout
//------------------------------------------------------------------------------

SchemaLayout::SchemaLayout(Schema *schema, QWidget* parent) : Z::GraphicsView(parent), _schema(schema)
{
    _axis = new OpticalAxisLayout::Layout(nullptr);
    _axis->setZValue(1000);
    _scene.addItem(_axis);

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
    _elemLayouts.insert(elem->element(), elem);
    _scene.addItem(elem);

    // Add element label
    if (elem->element()->layoutOptions.showLabel) {
        QGraphicsTextItem *label = _scene.addText(elem->element()->label());
        label->setZValue(1000 + _elements.count());
        label->setFont(getLabelFont());
        label->setToolTip(elem->toolTip());
        if (not _defaultLabelColor.isValid())
            _defaultLabelColor = label->defaultTextColor();
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
    _elemLayouts.clear();
    _scene.addItem(_axis);
    _elements.clear();
}

void SchemaLayout::centerView(const QRectF& rect)
{
    QRectF r = rect.isEmpty() ? _scene.itemsBoundingRect(): rect;
    centerOn(r.center());
}

const QFont& SchemaLayout::getLabelFont() const
{
    static QFont f = Z::Gui::ElemLabelFont().get();
    return f;
}

void SchemaLayout::updateSelection(const Elements& selected)
{
    setUpdatesEnabled(false);

    for (auto layout : _elements)
        layout->setSelected(false);
    for (auto it = _elemLabels.constBegin(); it != _elemLabels.constEnd(); it++)
        it.value()->setDefaultTextColor(_defaultLabelColor);
    for (auto elem : selected)
        if (auto layout = _elemLayouts.value(elem); layout) {
            layout->setSelected(true);
            if (auto label = _elemLabels.value(layout); label)
                label->setDefaultTextColor(_selectedLabelColor);
        }

    setUpdatesEnabled(true);
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

using LayoutFactory = QMap<QString, std::function<ElementLayout*(Element*)>>;
using OptionsFactory = QMap<QString, std::function<ElementLayoutOptionsView*()>>;
Q_GLOBAL_STATIC(LayoutFactory, __layoutFactoryMethods);
Q_GLOBAL_STATIC(OptionsFactory, __optionsFactoryMethods);

template <class TElement, class TLayout> void registerLayout() {
    TElement tmp;
    __layoutFactoryMethods->insert(tmp.type(), [](Element*e) { return new TLayout(e); });
}

template <class TElement, class TOptions> void registerLayoutOptions() {
    TElement tmp;
    __optionsFactoryMethods->insert(tmp.type(), []() { return new TOptions(); });
}

ElementLayout* make(Element *elem) {
    if (__layoutFactoryMethods->empty()) {
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
    if (!__layoutFactoryMethods->contains(elem->type()))
        return nullptr;
    return __layoutFactoryMethods->value(elem->type())(elem);
}

ElementLayoutOptionsView* getOptions(Element *elem)
{
    if (__optionsFactoryMethods->empty())
    {
        registerLayoutOptions<ElemBrewsterCrystal, ElemBrewsterCrystalLayout::LayoutOptions>();
        registerLayoutOptions<ElemBrewsterPlate, ElemBrewsterPlateLayout::LayoutOptions>();
        registerLayoutOptions<ElemEmptyRange, ElemEmptyRangeLayout::LayoutOptions>();
        registerLayoutOptions<ElemGrinLens, ElemGrinLensLayout::LayoutOptions>();
        registerLayoutOptions<ElemGrinMedium, ElemGrinMediumLayout::LayoutOptions>();
        registerLayoutOptions<ElemMediumRange, ElemMediumRangeLayout::LayoutOptions>();
        registerLayoutOptions<ElemPlate, ElemPlateLayout::LayoutOptions>();
        registerLayoutOptions<ElemPoint, ElemPointLayout::LayoutOptions>();
        registerLayoutOptions<ElemTiltedPlate, ElemTiltedPlateLayout::LayoutOptions>();
    }
    if (!__optionsFactoryMethods->contains(elem->type()))
        return nullptr;
    return __optionsFactoryMethods->value(elem->type())();
}

} // namespace ElementLayoutFactory
