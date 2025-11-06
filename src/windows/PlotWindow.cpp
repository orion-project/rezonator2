#include "PlotWindow.h"

#include <QMenu>

#include "tools/OriPetname.h"

#include "qcpl_utils.h"
#include "qcpl_plot.h"

//------------------------------------------------------------------------------
//                             PlotWindowStorable
//------------------------------------------------------------------------------

namespace PlotWindowStorable
{

SchemaWindow* createWindow(Schema* schema)
{
    return PlotWindow::create(schema);
}

} // namespace PlotWindowStorable

//------------------------------------------------------------------------------
//                                PlotWindow
//------------------------------------------------------------------------------

static int __plotCount = 0;

PlotWindow* PlotWindow::create(Schema* schema)
{
    return new PlotWindow(schema);
}

PlotWindow::PlotWindow(Schema* schema) : PlotBaseWindow(schema)
{
    setTitleAndIcon(tr("Plot (%1)").arg(++__plotCount), ":/toolbar/gauss_far_zone");
    
    menuPlot->addAction(tr("Add Random Sample"), this, &PlotWindow::addRandomSample);
    menuPlot->addAction(tr("Add Random Sample (multisegment)"), this, &PlotWindow::addRandomSampleMultisegment);
    
    updateCursorInfo();
}

void PlotWindow::addRandomSample()
{
    auto g = _plot->makeNewGraph(OriPetname::make(), QCPL::makeRandomSample());
    g->addToLegend();
    _plot->autolimits();
}

void PlotWindow::addRandomSampleMultisegment()
{
    QString name = OriPetname::make();

    auto d = QCPL::makeRandomSample();
    auto g = _plot->makeNewGraph(name, d);
    g->addToLegend();
    
    auto d1 = QCPL::makeRandomSample();
    double offset = d.x.last() + 25;
    for (double &x : d1.x)
        x+= offset;
    auto g1 = _plot->makeNewGraph(name, d1);
    g1->setPen(g->pen());
    
    _plot->autolimits();
}
