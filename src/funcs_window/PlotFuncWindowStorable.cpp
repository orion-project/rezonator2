#include "PlotFuncWindowStorable.h"

#include "../widgets/CursorPanel.h"

#include <QAction>
#include <QJsonObject>

#define TAG_TS_MODE "mode"
#define TS_MODE_TS_FLIP "T-S"
#define TS_MODE_T_AND_S "T+S"
#define TS_MODE_T "T"
#define TS_MODE_S "S"

PlotFuncWindowStorable::PlotFuncWindowStorable(PlotFunction *func) : PlotFuncWindow(func)
{
}

QString PlotFuncWindowStorable::read(const QJsonObject &root)
{
    auto funcJson = root["function"].toObject();
    QString res = readFunction(funcJson);
    if (!res.isEmpty()) return res;

    auto wndJson = root["window"].toObject();
    res = readWindowGeneral(wndJson);
    if (!res.isEmpty()) return res;

    res = readWindowSpecific(wndJson);
    if (!res.isEmpty()) return res;

   return QString();
}

QString PlotFuncWindowStorable::write(QJsonObject &root)
{
    QJsonObject funcJson;
    QString res = writeFunction(funcJson);
    if (!res.isEmpty()) return res;

    QJsonObject wndJson;
    res = writeWindowGeneral(wndJson);
    if (!res.isEmpty()) return res;

    res = writeWindowSpecific(wndJson);
    if (!res.isEmpty()) return res;

    root["function"] = funcJson;
    root["window"] = wndJson;

    return QString();
}

QString PlotFuncWindowStorable::readWindowGeneral(const QJsonObject& root)
{
    readTSMode(root);
    readCursor(root);
    return QString();
}

QString PlotFuncWindowStorable::writeWindowGeneral(QJsonObject& root) const
{
    writeTSMode(root);
    writeCursor(root);
    return QString();
}

void PlotFuncWindowStorable::readTSMode(const QJsonObject& root)
{/*
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
*/}

void PlotFuncWindowStorable::readCursor(const QJsonObject& root)
{/*
    QDomElement node;
    if (reader->getNode(root, "cursor", node))
    {
        _cursorPanel->setEnabled(reader->readBoolAttributeDef(node, "enabled", true));
        _cursorPanel->setMode(CursorPanel::Mode(reader->readIntAttributeDef(node, "mode", CursorPanel::Both)));
    }
*/}

void PlotFuncWindowStorable::writeTSMode(QJsonObject& root) const
{/*
    auto node = writer->makeNode(root, "ts-mode");
    bool modeT = actnShowT->isChecked();
    bool modeS = actnShowS->isChecked();
    node.setAttribute("mode", (modeT && modeS)? "T+S": (modeT ? "T" : "S"));
    node.setAttribute("flipped", actnShowTS->isChecked());
*/}

void PlotFuncWindowStorable::writeCursor(QJsonObject& root) const
{/*
    auto node = writer->makeNode(root, "cursor");
    node.setAttribute("enabled", _cursorPanel->enabled());
    node.setAttribute("mode", _cursorPanel->mode());
*/}

