#ifndef PLOT_WINDOW_H
#define PLOT_WINDOW_H

#include "PlotBaseWindow.h"
#include "../io/ISchemaWindowStorable.h"

/**
    Implementation of restoreability for @a PlotWindow.
    Register it in @a ProjectWindow::registerStorableWindows().
*/
namespace PlotWindowStorable
{
inline QString windowType() { return "PlotWindow"; }
SchemaWindow* createWindow(Schema* schema);
}

/**
    Generic plot window displaying arbitrary graphs.
*/
class PlotWindow : public PlotBaseWindow, public ISchemaWindowStorable
{
    Q_OBJECT

public:
    static PlotWindow* create(Schema*);
    
    // inherits from ISchemaWindowStorable
    QString storableType() const override { return PlotWindowStorable::windowType(); }

private:
    explicit PlotWindow(Schema*);
    
    void addRandomSample();
    void addRandomSampleMultisegment();
};

#endif // PLOT_WINDOW_H