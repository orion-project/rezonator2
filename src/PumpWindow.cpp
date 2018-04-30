#include "PumpWindow.h"

#include "widgets/ValuesEditorTS.h"
#include "widgets/OriOptionsGroup.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriDialogs.h"

#include <QLabel>
#include <QListWidget>
#include <QTimer>

namespace PumpWindowStorable
{
    SchemaWindow* createWindow(Schema* schema)
    {
        return PumpWindow::create(schema);
    }
} // namespace PumpWindowStorable

//------------------------------------------------------------------------------
//                                PumpWindow
//------------------------------------------------------------------------------

PumpWindow* PumpWindow::_instance = nullptr;

PumpWindow* PumpWindow::create(Schema* owner)
{
    if (!_instance)
        _instance = new PumpWindow(owner);
    return _instance;
}

PumpWindow::PumpWindow(Schema *owner) : SchemaMdiChild(owner)
{
    setTitleAndIcon(tr("Pumps"), ":/window_icons/pump");

    //schema()->registerListener(_table);
}

PumpWindow::~PumpWindow()
{
    _instance = nullptr;

    //schema()->unregisterListener(_table);
}
