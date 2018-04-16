#include "PlotFuncWindowStorable.h"

#include "../widgets/CursorPanel.h"
#include "../io/z_io_utils.h"

#include <QAction>
#include <QJsonObject>

QString PlotFuncWindowStorable::storableRead(const QJsonObject &root)
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

QString PlotFuncWindowStorable::storableWrite(QJsonObject &root)
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
    auto mode = root["ts_mode"].toString();
    bool modeT = true, modeS = true;
    if (mode == "T") modeS = false;
    else if (mode == "S") modeT = false;
    actnShowT->setChecked(modeT);
    actnShowS->setChecked(modeS);
    actnShowTS->setChecked(root["ts_flipped"].toBool());
    updateTSModeActions();
    updateVisibilityTS();

    _cursorPanel->setEnabled(root["cursor_enabled"].toBool(true));
    _cursorPanel->setMode(Z::IO::Utils::enumFromStr(root["cursor_mode"].toString(), CursorPanel::Both));

    return QString();
}

QString PlotFuncWindowStorable::writeWindowGeneral(QJsonObject& root) const
{
    bool modeT = actnShowT->isChecked();
    bool modeS = actnShowS->isChecked();
    root["ts_mode"] = (modeT && modeS)? "T+S": (modeT ? "T" : "S");
    root["ts_flipped"] = actnShowTS->isChecked();

    root["cursor_enabled"] = _cursorPanel->enabled();
    root["cursor_mode"] = Z::IO::Utils::enumToStr(_cursorPanel->mode());

    return QString();
}
