#include "PlotFuncWindowStorable.h"

#include "../core/Report.h"
#include "../io/CommonUtils.h"
#include "../math/FunctionGraph.h"

#include "qcpl_cursor.h"
#include "qcpl_cursor_panel.h"
#include "qcpl_io_json.h"
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
    res = readWindowGeneral(wndJson, report);
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

    updatePlotItemToggleActions();
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

QString PlotFuncWindowStorable::readWindowGeneral(const QJsonObject& root, Z::Report *report)
{
    // Restore graphs visibility
    auto modeTS = root["ts_mode"].toString();
    bool modeT = true, modeS = true;
    if (modeTS == "T") modeS = false;
    else if (modeTS == "S") modeT = false;
    actnShowT->setChecked(modeT);
    actnShowS->setChecked(modeS);
    actnShowFlippedTS->setChecked(root["ts_flipped"].toBool());
    showModeTS();

    // Restore cursor state
    _cursorPanel->setEnabled(root["cursor_enabled"].toBool(true));
    _cursorPanel->setMode(Z::IO::Utils::enumFromStr(root["cursor_mode"].toString(), QCPL::CursorPanel::Both));
    if (root.contains("cursor_pen"))
    {
        _cursorPen = QCPL::readPen(root["cursor_pen"].toObject(), _cursor->pen());
        _cursor->setPen(_cursorPen.value());
    }
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
    _plot->setFormatterTextX(root["x_title"].toString());
    _plot->setFormatterTextY(root["y_title"].toString());
    _plot->setFormatterTextT(root["t_title"].toString());

    // Restore plot format
    QCPL::JsonReport qcpl_report;
    QCPL::readPlot(root["format"].toObject(), _plot, &qcpl_report);
    for (auto& msg : qcpl_report)
        if (!msg.ok() && msg.code != QCPL::JsonError::NoData)
            report->warning(msg.message);
    if (root.contains("pen_t"))
    {
        _graphPenT = QCPL::readPen(root["pen_t"].toObject(), _graphs->T()->pen());
        _graphs->T()->setPen(_graphPenT.value());
    }
    if (root.contains("pen_s"))
    {
        _graphPenS = QCPL::readPen(root["pen_s"].toObject(), _graphs->S()->pen());
        _graphs->S()->setPen(_graphPenS.value());
    }

    // Restore view states
    auto viewsJson = root["stored_views"].toObject();
    auto mode = viewsJson.constBegin();
    while (mode != viewsJson.constEnd())
    {
        ViewSettings vs;
        auto viewJson = mode.value().toObject();
        auto it = viewJson.constBegin();
        while (it != viewJson.constEnd())
        {
            vs[it.key()] = it.value().toVariant();
            it++;
        }
        _storedView[mode.key().toInt()] = vs;
        mode++;
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
    if (_cursorPen.has_value())
        root["cursor_pen"] = QCPL::writePen(_cursorPen.value());

    // Store plot limits
    auto limitsX = _plot->limitsX();
    auto limitsY = _plot->limitsY();
    root["x_min"] = limitsX.min;
    root["x_max"] = limitsX.max;
    root["x_unit"] = getUnitX()->alias();
    root["x_title"] = _plot->formatterTextX();
    root["y_min"] = limitsY.min;
    root["y_max"] = limitsY.max;
    root["y_unit"] = getUnitY()->alias();
    root["y_title"] = _plot->formatterTextY();
    root["t_title"] = _plot->formatterTextT();

    root["format"] = QCPL::writePlot(_plot);
    if (_graphPenT.has_value())
        root["pen_t"] = QCPL::writePen(_graphPenT.value());
    if (_graphPenS.has_value())
        root["pen_s"] = QCPL::writePen(_graphPenS.value());

    // Store view states
    QJsonObject viewsJson;
    auto mode = _storedView.constBegin();
    while (mode != _storedView.constEnd())
    {
        const auto& view = mode.value();
        QJsonObject viewJson;
        auto it = view.constBegin();
        while (it != view.constEnd())
        {
            viewJson[it.key()] = QJsonValue::fromVariant(it.value());
            it++;
        }
        viewsJson[QString::number(mode.key())] = viewJson;
        mode++;
    }
    root["stored_views"] = viewsJson;

    return QString();
}
