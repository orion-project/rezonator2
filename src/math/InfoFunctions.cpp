#include "InfoFunctions.h"

#include "FormatInfo.h"
#include "RoundTripCalculator.h"
#include "../app/AppSettings.h"
#include "../app/Appearance.h"
#include "../core/Format.h"
#include "../core/Schema.h"
#include "../widgets/ValueEditor.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriDialogs.h"

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>

//------------------------------------------------------------------------------
//                              InfoFuncMatrix
//------------------------------------------------------------------------------

InfoFuncMatrix::InfoFuncMatrix(Schema *schema, Element *elem)
    : InfoFunction(schema), _element(elem) {}

FunctionBase::FunctionState InfoFuncMatrix::elementDeleting(Element *elem)
{
    if (_schema->count() == 1) return Dead;
    return _element == elem? Frozen: Ok;
}

QString InfoFuncMatrix::calculateInternal()
{
    QString report = Z::Format::elementTitleAndMatrices(_element);
    if (AppSettings::instance().showPythonMatrices)
        report += Z::Format::Py::elementMatrices(_element);
    return report;
}

//------------------------------------------------------------------------------
//                             InfoFuncMatrices
//------------------------------------------------------------------------------

InfoFuncMatrices::InfoFuncMatrices(Schema *schema, const Elements& elems)
    : InfoFunction(schema), _elements(elems) {}

FunctionBase::FunctionState InfoFuncMatrices::elementDeleting(Element *elem)
{
    _elements.removeAll(elem);
    return _elements.isEmpty()? Dead: Ok;
}

QString InfoFuncMatrices::calculateInternal()
{
    QString result;
    QTextStream stream(&result);
    for (int i = 0; i < _elements.size(); i++)
    {
        stream << Z::Format::elementTitleAndMatrices(_elements.at(i));
        if (AppSettings::instance().showPythonMatrices)
            stream << Z::Format::Py::elementMatrices(_elements.at(i));
        if (i < _elements.size() - 1)
            stream << "<hr>";
    }
    return result;
}

//------------------------------------------------------------------------------
//                             InfoFuncMatrixMult
//------------------------------------------------------------------------------

InfoFuncMatrixMult::InfoFuncMatrixMult(Schema *schema, const Elements& elems)
    : InfoFuncMatrices(schema, elems)
{
    {
        InfoFuncAction a;
        a.title = qApp->translate("InfoFuncMatrixMultFwd", "Multiply in order of selection");
        a.showInMenu = true;
        a.triggered = [this](){ _useSelectionOrder = !_useSelectionOrder; calculate(); };
        a.isChecked = [this](){ return _useSelectionOrder; };
        _actions << a;
    }
    {
        InfoFuncAction a;
        a.title = qApp->translate("InfoFuncMatrixMultFwd", "Multiply in reverse order");
        a.showInMenu = true;
        a.triggered = [this](){ _useReverseOrder = !_useReverseOrder; calculate(); };
        a.isChecked = [this](){ return _useReverseOrder; };
        _actions << a;
    }
    {
        InfoFuncAction a;
        a.title = qApp->translate("InfoFuncMatrixMultFwd", "Use back proparation matrices");
        a.showInMenu = true;
        a.triggered = [this](){ _useInvMatrs = !_useInvMatrs; calculate(); };
        a.isChecked = [this](){ return _useInvMatrs; };
        _actions << a;
    }
    {
        InfoFuncAction a;
        a.title = qApp->translate("InfoFuncMatrixMultFwd", "Use disabled elements");
        a.showInMenu = true;
        a.triggered = [this](){ _useDisabledElems = !_useDisabledElems; calculate(); };
        a.isChecked = [this](){ return _useDisabledElems; };
        _actions << a;
    }
}

QString InfoFuncMatrixMult::calculateInternal()
{
    // _elements are in the selection order

    Elements elems0;
    if (!_useSelectionOrder) {
        for (const auto elem : _schema->elements()) {
            if (_elements.contains(elem))
                elems0 << elem;
        }
    } else {
        elems0 = _elements;
    }
    Elements elems1;
    if (_useReverseOrder) {
        elems1.reserve(elems0.size());
        std::reverse_copy(elems0.begin(), elems0.end(), std::back_inserter(elems1));
    } else {
        elems1 = elems0;
    }
    Elements elems2;
    if (!_useDisabledElems) {
        for (const auto elem : std::as_const(elems1))
            if (!elem->disabled())
                elems2 << elem;
    } else {
        elems2 = elems1;
    }
    
    Z::Matrix mt, ms;
    for (const auto elem : std::as_const(elems2))
    {
        mt *= _useInvMatrs ? elem->Mt_inv() : elem->Mt() ;
        ms *= _useInvMatrs ? elem->Ms_inv() : elem->Ms();
    }

    QString report = QStringLiteral("%1:<p>%2")
        .arg(Z::Format::roundTrip(elems2, true), Z::Format::matrices(mt, ms));

    if (AppSettings::instance().showPythonMatrices)
        report += Z::Format::Py::roundTrip(elems2);

    return report;
}

//------------------------------------------------------------------------------
//                            InfoFuncMatrixRT
//------------------------------------------------------------------------------

InfoFuncMatrixRT::InfoFuncMatrixRT(Schema *schema, Element *elem)
    : InfoFunction(schema), _element(elem)
{
    InfoFuncAction a;
    a.title = qApp->translate("InfoFuncMatrixRT", "Show all element matrices");
    a.icon = ":/toolbar/elem_matr";
    a.triggered = [this](){ _showElems = !_showElems; calculate(); };
    a.isChecked = [this](){ return _showElems; };
    _actions << a;

    auto range = elem->asRange();
    if (range)
    {
        InfoFuncAction act;
        act.title = qApp->translate("InfoFuncMatrixRT", "Set reference plane offset inside element");
        act.icon = ":/toolbar/ref_offset";
        act.triggered = [this](){ setRefOffset(); };
        _actions << act;
        _refOffset = {0, range->paramLength()->value().unit()};
    }
}

FunctionBase::FunctionState InfoFuncMatrixRT::elementDeleting(Element *elem)
{
    if (_schema->count() == 1) return Dead;
    return _element == elem? Frozen: Ok;
}

QString InfoFuncMatrixRT::calculateInternal()
{
    auto range = _element->asRange();
    bool useOffset = range && _useRefOffset;

    RoundTripCalculator c(_schema, _element);
    c.calcRoundTrip(useOffset);
    if (c.isEmpty())
    {
        QString msg = qApp->translate("Calc error", "Round-trip is empty");
        if (!c.error().isEmpty()) msg += ": " + c.error();
        return msg;
    }

    double backupSubRange;
    if (useOffset)
    {
        backupSubRange = range->subRangeSI();
        range->setSubRange(_refOffset);
    }

    c.multMatrix("InfoFuncMatrixRT::calculateInternal");

    QString res = format(&c, _showElems);

    if (useOffset)
        range->setSubRangeSI(backupSubRange);

    return res;
}

void InfoFuncMatrixRT::setRefOffset()
{
    auto range = _element->asRange();

    auto takeWhole = new QRadioButton(qApp->translate("InfoFuncMatrixRT", "Take whole element"));
    auto useOffset = new QRadioButton(qApp->translate("InfoFuncMatrixRT", "Use offset inside element"));
    takeWhole->setChecked(!_useRefOffset);
    useOffset->setChecked(_useRefOffset);

    auto lengthLabel = new QLabel;
    lengthLabel->setFont(Z::Gui::ValueFont().get());
    lengthLabel->setText(range->axisLen().displayStr());

    auto offsetEditor = new ValueEditor;
    offsetEditor->setValue(_refOffset);
    qApp->connect(offsetEditor, &ValueEditor::valueChanged, [useOffset](){ useOffset->setChecked(true); });

    auto layout = new QGridLayout;
    layout->addWidget(new QLabel(qApp->translate("InfoFuncMatrixRT", "Axial length")), 1, 0); layout->addWidget(lengthLabel, 1, 1);
    layout->addWidget(new QLabel(qApp->translate("InfoFuncMatrixRT", "Ref. plane offset")), 2, 0); layout->addWidget(offsetEditor, 2, 1);

    auto w = Ori::Layouts::LayoutV({takeWhole, useOffset, Ori::Layouts::Space(6), layout, Ori::Layouts::Space(12)}).setMargin(0).makeWidget();
    if (Ori::Dlg::Dialog(w, false).exec())
    {
        _useRefOffset = useOffset->isChecked();
        _refOffset = offsetEditor->value();
        calculate();
    }
    delete w;
}

QString InfoFuncMatrixRT::format(RoundTripCalculator *c, bool showElems)
{
    QString result;
    QTextStream stream(&result);
    stream << Z::Format::roundTrip(c->roundTrip(), true) << QChar(':')
           << Z::Format::matrices(c->Mt(), c->Ms())
           << QStringLiteral("<p><span class=param>Ref:&nbsp;</span>")
           << Z::Format::linkViewMatrix(c->reference());

    if (c->splitRange())
    {
        auto range = c->reference()->asRange();
        if (range)
            stream << " (offset inside: " << range->subRangeLf().displayStr() << ")";
    }

    if (c->owner()->isResonator())
    {
        c->setStabilityCalcMode(Z::Enums::StabilityCalcMode::Normal);
        auto stabNormal = c->stabilityCplx();
        stream << "<p>Stability (normal): " << formatStability('T', stabNormal.T) << formatStability('S', stabNormal.S);
        c->setStabilityCalcMode(Z::Enums::StabilityCalcMode::Squared);
        auto stabSquared = c->stabilityCplx();
        stream << "<br>Stability (squared): " << formatStability('T', stabSquared.T) << formatStability('S', stabSquared.S);
    }

    QString resultPy;
    QTextStream streamPy(&resultPy);
    QStringList namesPyT, namesPyS;

    auto matrsT = c->matrsT();
    auto matrsS = c->matrsS();
    auto info = c->matrixInfo();
    int count = matrsT.size();
    for (int i = 0; i < count; i++)
    {
        auto mi = info.at(i);

        // Output element matrices
        if (showElems)
        {
            stream << QStringLiteral("<hr>");

            stream << QStringLiteral("<span class=elem_label>")
                   << mi.owner->displayLabel()
                   << QStringLiteral("</span>");

            if (!mi.owner->title().isEmpty())
                stream << QStringLiteral(" <span class=elem_title>(")
                       << mi.owner->title()
                       << QStringLiteral(")</span>");

            if (mi.kind == mi.LEFT_HALF)
            {
                auto range = mi.owner->asRange();
                if (range)
                    stream << " (left part: " << range->subRangeLf().displayStr() << ")";
            }
            else if (mi.kind == mi.RIGHT_HALF)
            {
                auto range = mi.owner->asRange();
                if (range)
                    stream << " (right part: " << range->subRangeRt().displayStr() << ")";
            }
            else if (mi.kind == mi.BACK_PASS)
                stream << " (back pass)";

            stream << Z::Format::matrices(matrsT.at(i), matrsS.at(i));
        }

        // Collect variable names for round-trip expression
        if (AppSettings::instance().showPythonMatrices)
        {
            QString nameT = Z::Format::Py::matrixVarName(mi.owner, "_t");
            QString nameS = Z::Format::Py::matrixVarName(mi.owner, "_s");
            if (mi.kind == mi.LEFT_HALF)
            {
                nameT += "_lf";
                nameS += "_lf";
            }
            else if (mi.kind == mi.RIGHT_HALF)
            {
                nameT += "_rt";
                nameS += "_rt";
            }
            else if (mi.kind == mi.BACK_PASS)
            {
                nameT += "_inv";
                nameS += "_inv";
            }
            namesPyT << nameT;
            namesPyS << nameS;

            // Output element matrices as py-code
            if (showElems)
            {
                stream << "<p><code>"
                       << Z::Format::Py::matrixAsNumpy(nameT, matrsT.at(i)) << "<br>"
                       << Z::Format::Py::matrixAsNumpy(nameS, matrsS.at(i))
                       << "</code></p>";
                streamPy
                       << Z::Format::Py::matrixAsNumpy(nameT, matrsT.at(i)) << "<br>"
                       << Z::Format::Py::matrixAsNumpy(nameS, matrsS.at(i)) << "<br><br>";
            }
        }
    }
    if (AppSettings::instance().showPythonMatrices)
    {
        streamPy << "# Round-trip matrices<br>"
                 << "M0_t = " << namesPyT.join(" * ") << "<br>"
                 << "M0_s = " << namesPyS.join(" * ") << "<br>";

        stream << "<hr><p><code>" << resultPy << "</code>";
    }

    return result;
}

QString InfoFuncMatrixRT::formatStability(char plane, const Z::Complex& v)
{
    QString s = Z::isReal(v) ? Z::format(v.real()) : Z::format(v);
    return QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                          "<span class=param>P<sub>%1</sub></span>"
                          "<span class=value> = %2</span>")
            .arg(plane).arg(s);
}

//------------------------------------------------------------------------------
//                            InfoFuncRepetitionRate
//------------------------------------------------------------------------------

QString InfoFuncRepetitionRate::calculateInternal()
{
    _repetitonRate = 0;
    double L = 0;
    int count = 0;
    QStringList parts;
    for (auto elem : schema()->elements())
    {
        auto range = elem->asRange();
        if (range && !elem->disabled())
        {
            count++;
            L += range->opticalPathSI();

            QString mult_ior;
            if (Double(range->ior()).isNot(1))
            {
                mult_ior = Z::Strs::multX() % Z::format(range->ior());
                count++; // when n != 1 then the sum should be printed even if it is the only element
            }
            parts += QString::fromLatin1("<b>%1</b>%2<font color=gray><i>(%3)</i></font>")
                .arg(Z::format(range->axisLengthSI()), mult_ior, elem->displayLabel());
        }
    }

    if (Double(L).is(0))
        return qApp->translate("Func", "The schema does not contain elements having "
                "length (ranges, crystals, etc.), or their total length is zero.");

    _repetitonRate = Z::Const::LightSpeed / L;
    if (schema()->isSW()) _repetitonRate /= 2.0;

    Z::PrefixedValue freq(_repetitonRate, Z::Units::Hz()); // convert to suitable kHz, MHz, etc.

    QString sum = "<b>" % Z::format(L) % Z::Units::m()->name() % "</b>";
    if (count > 1) sum = parts.join(" + ") % " = " % sum;

#ifdef Q_OS_MAC
    const int freqFontSize = 16;
#else
    const int freqFontSize = 12;
#endif
    return QStringLiteral("<p style='font-size:%1pt'><b>%2</b>"
                          "<p>%3:<br>%4")
            .arg(freqFontSize)
            .arg(freq.format())
            .arg(qApp->translate("Func", "Total cavity length"))
            .arg(sum);
}

//------------------------------------------------------------------------------
//                             InfoFuncSummary
//------------------------------------------------------------------------------

QString InfoFuncSummary::calculateInternal()
{
    Z::Format::FormatElemParams formatParams;
    formatParams.schema = schema();

    QStringList report;
    report << QStringLiteral("<table border=1 cellspacing=-1 style='border-color:#aaa;border-style:solid'>");
    for (Element* elem : schema()->elements())
        if (!elem->disabled() && elem->hasParams())
        {
            report << QStringLiteral("<tr><td style='padding:2 4'><span style='")
                   << Z::Gui::html(Z::Gui::ElemLabelFont())
                   << QStringLiteral("'>")
                   << elem->displayLabel()

                   // weird, but the space after span is important!
                   // without it vertical alignment calculated improperly sometimes
                   << QStringLiteral("</span> </td><td style='padding:2 4'>")

                   << formatParams.format(elem)
                   << QStringLiteral("</td><td style='padding:2 4'>")
                   << elem->title()
                   << QStringLiteral("</td></tr>");
        }
    report << QStringLiteral("</table><br>");

    Z::Format::FormatParam formatWavelength;
    formatWavelength.includeDriver = false;
    formatWavelength.includeValue = true;
    report << QStringLiteral("<br>")
           << formatWavelength.format(&schema()->wavelength())
           << QStringLiteral("<br>");

    if (schema()->isSP())
    {
        auto pump = schema()->activePump();
        if (pump)
        {
            report << QStringLiteral("<br>")
                   << qApp->translate("InfoFuncSummary", "<b>Input beam:</b>")
                   << QStringLiteral("<br>");

            auto pumpMode = Pumps::findByModeName(pump->modeName());
            if (pumpMode)
                report << pumpMode->description()
                       << QStringLiteral("<br>");

            report << Z::Format::FormatPumpParams().format(pump);
        }
    }

    return report.join(QString());
}
