#include "PlotFuncWindowStorable.h"

#include "../io/CommonUtils.h"
#include "../core/Report.h"

#include "qcpl_cursor_panel.h"
#include "qcpl_plot.h"

#include <QAction>
#include <QJsonObject>

bool PlotFuncWindowStorable::storableRead(const QJsonObject &root, Z::Report *report)
{
    auto funcJson = root["function"].toObject();
    QString res = readFunction(funcJson);
    if (!res.isEmpty())
    {
        report->warning(res);
        return false;
    }

    auto wndJson = root["window"].toObject();
    res = readWindowGeneral(wndJson);
    if (!res.isEmpty())
    {
        report->warning(res);
        return false;
    }

    res = readWindowSpecific(wndJson);
    if (!res.isEmpty())
    {
        report->warning(res);
        return false;
    }

   return true;
}

bool PlotFuncWindowStorable::storableWrite(QJsonObject &root, Z::Report *report)
{
    QJsonObject funcJson;
    QString res = writeFunction(funcJson);
    if (!res.isEmpty())
    {
        report->warning(res);
        return false;
    }

    QJsonObject wndJson;
    res = writeWindowGeneral(wndJson);
    if (!res.isEmpty())
    {
        report->warning(res);
        return false;
    }

    res = writeWindowSpecific(wndJson);
    if (!res.isEmpty())
    {
        report->warning(res);
        return false;
    }

    root["function"] = funcJson;
    root["window"] = wndJson;

    return true;
}

QString PlotFuncWindowStorable::readWindowGeneral(const QJsonObject& root)
{
    // Restore graphs visibility
    auto mode = root["ts_mode"].toString();
    bool modeT = true, modeS = true;
    if (mode == "T") modeS = false;
    else if (mode == "S") modeT = false;
    actnShowT->setChecked(modeT);
    actnShowS->setChecked(modeS);
    actnShowFlippedTS->setChecked(root["ts_flipped"].toBool());
    showModeTS();

    // Restore cursor state
    _cursorPanel->setEnabled(root["cursor_enabled"].toBool(true));
    _cursorPanel->setMode(Z::IO::Utils::enumFromStr(root["cursor_mode"].toString(), QCPL::CursorPanel::Both));
    requestCenterCursor();

    // Restore plot limits
    QCPL::AxisLimits limitsX { root["x_min"].toDouble(Double::nan()),
                               root["x_max"].toDouble(Double::nan()) };
    QCPL::AxisLimits limitsY { root["y_min"].toDouble(Double::nan()),
                               root["y_max"].toDouble(Double::nan()) };
    if (limitsX.isInvalid() || limitsY.isInvalid())
        requestAutolimits();
    else
    {
        _plot->setLimitsX(limitsX, false);
        _plot->setLimitsY(limitsY, false);
    }
    _unitX = Z::Units::findByAlias(root["x_unit"].toString(), Z::Units::none());
    _unitY = Z::Units::findByAlias(root["y_unit"].toString(), Z::Units::none());

    // Restore view states
    QJsonArray viewsJson = root["stored_views"].toArray();
    for (auto it = viewsJson.begin(); it != viewsJson.end(); it++)
    {
        QJsonObject viewJson = (*it).toObject();
        auto mode = viewJson["mode"].toInt(-1);
        if (mode < 0) continue;
        ViewState state;
        state.limitsX = { viewJson["x_min"].toDouble(Double::nan()),
                          viewJson["x_max"].toDouble(Double::nan()) };
        state.limitsY = { viewJson["y_min"].toDouble(Double::nan()),
                          viewJson["y_max"].toDouble(Double::nan()) };
        if (state.limitsX.isInvalid() or state.limitsY.isInvalid())
            continue;
        state.unitX = Z::Units::findByAlias(viewJson["x_unit"].toString(), Z::Units::none());
        state.unitY = Z::Units::findByAlias(viewJson["y_unit"].toString(), Z::Units::none());
        _storedView.insert(mode, state);
    }

    return QString();
}

QString PlotFuncWindowStorable::writeWindowGeneral(QJsonObject& root) const
{
    // Store graphs visibility
    bool modeT = actnShowT->isChecked();
    bool modeS = actnShowS->isChecked();
    root["ts_mode"] = (modeT && modeS)? "T+S": (modeT ? "T" : "S");
    root["ts_flipped"] = actnShowFlippedTS->isChecked();

    // Store cursor state
    root["cursor_enabled"] = _cursorPanel->enabled();
    root["cursor_mode"] = Z::IO::Utils::enumToStr(_cursorPanel->mode());

    // Store plot limits
    auto limitsX = _plot->limitsX();
    auto limitsY = _plot->limitsY();
    root["x_min"] = limitsX.min;
    root["x_max"] = limitsX.max;
    root["x_unit"] = getUnitX()->alias();
    root["y_min"] = limitsY.min;
    root["y_max"] = limitsY.max;
    root["y_unit"] = getUnitY()->alias();

    // Store view states
    QJsonArray viewsJson;
    for (int key : _storedView.keys())
    {
        auto view = _storedView[key];
        viewsJson.append(QJsonObject{
            { "mode", key },
            { "x_min", view.limitsX.min },
            { "x_max", view.limitsX.max },
            { "x_unit", view.unitX->alias() },
            { "y_min", view.limitsY.min },
            { "y_max", view.limitsY.max },
            { "y_unit", view.unitY->alias() },
        });
    }
    root["stored_views"] = viewsJson;

    return QString();
}
