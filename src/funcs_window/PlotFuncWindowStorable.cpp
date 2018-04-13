#include "PlotFuncWindowStorable.h"

#include "../widgets/CursorPanel.h"

#include <QAction>

#define TAG_TS_MODE "mode"
#define TS_MODE_TS_FLIP "T-S"
#define TS_MODE_T_AND_S "T+S"
#define TS_MODE_T "T"
#define TS_MODE_S "S"

PlotFuncWindowStorable::PlotFuncWindowStorable(PlotFunction *func) : PlotFuncWindow(func)
{
}

bool PlotFuncWindowStorable::read(QJsonObject &root)
{
    /* TODO
    QDomElement nodeFunc, nodeWnd;
    return
        root->getNode(root, "function", nodeFunc) &&
            readFunction(root, root) &&
        root->getNode(root, "window", nodeWnd) &&
            readWindowGeneral(root, nodeWnd) &&
            readWindowSpecific(root, nodeWnd);
            */
   return true;
}

void PlotFuncWindowStorable::write(QJsonObject &root)
{
    /* TODO
    auto nodeFunc = root->makeNode(root, "function");
    writeFunction(root, nodeFunc);

    auto nodeWnd = root->makeNode(root, "window");
    writeWindowGeneral(root, nodeWnd);
    writeWindowSpecific(root, nodeWnd);
    */
}
/*
bool PlotFuncWindowStorable::readWindowGeneral(Z::IO::XML::Reader* reader, QDomElement& root)
{
    readTSMode(reader, root);
    readCursor(reader, root);
    return true;
}

void PlotFuncWindowStorable::writeWindowGeneral(Z::IO::XML::Writer* writer, QDomElement& root) const
{
    writeTSMode(writer, root);
    writeCursor(writer, root);
}

void PlotFuncWindowStorable::readTSMode(Z::IO::XML::Reader* reader, QDomElement& root)
{
    QDomElement node;
    if (reader->getNode(root, "ts-mode", node))
    {
        auto mode = node.attribute("mode");
        if (mode == "T+S")
        {
            actnShowT->setChecked(true);
            actnShowS->setChecked(true);
        }
        else if (mode == "T")
        {
            actnShowT->setChecked(true);
            actnShowS->setChecked(false);
        }
        else
        {
            actnShowT->setChecked(false);
            actnShowS->setChecked(true);
        }
        actnShowTS->setChecked(reader->readBoolAttributeDef(node, "flipped", false));
    }
    else
    {
        actnShowT->setChecked(true);
        actnShowS->setChecked(true);
    }
    updateTSModeActions();
    updateVisibilityTS();
}

void PlotFuncWindowStorable::readCursor(Z::IO::XML::Reader* reader, QDomElement& root)
{
    QDomElement node;
    if (reader->getNode(root, "cursor", node))
    {
        _cursorPanel->setEnabled(reader->readBoolAttributeDef(node, "enabled", true));
        _cursorPanel->setMode(CursorPanel::Mode(reader->readIntAttributeDef(node, "mode", CursorPanel::Both)));
    }
}

void PlotFuncWindowStorable::writeTSMode(Z::IO::XML::Writer* writer, QDomElement& root) const
{
    auto node = writer->makeNode(root, "ts-mode");
    bool modeT = actnShowT->isChecked();
    bool modeS = actnShowS->isChecked();
    node.setAttribute("mode", (modeT && modeS)? "T+S": (modeT ? "T" : "S"));
    node.setAttribute("flipped", actnShowTS->isChecked());
}

void PlotFuncWindowStorable::writeCursor(Z::IO::XML::Writer* writer, QDomElement& root) const
{
    auto node = writer->makeNode(root, "cursor");
    node.setAttribute("enabled", _cursorPanel->enabled());
    node.setAttribute("mode", _cursorPanel->mode());
}
*/
