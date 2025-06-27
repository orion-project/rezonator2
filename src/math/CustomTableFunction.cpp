#include "CustomTableFunction.h"

#include "AbcdBeamCalculator.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../app/AppSettings.h"
#include "../core/PyRunner.h"

#include <QApplication>

#define COL_BEAMSIZE QStringLiteral("beamsize")
#define COL_APER_RATIO QStringLiteral("aper")
#define COL_WAVEFRONT QStringLiteral("wavefront")
#define COL_ANGLE QStringLiteral("angle")
#define COL_COUNT 3

CustomTableFunction::CustomTableFunction(Schema *schema) : TableFunction(schema)
{
}

QVector<TableFunction::ColumnDef> CustomTableFunction::columns() const
{
    ColumnDef beamRadius;
    beamRadius.id = COL_BEAMSIZE;
    beamRadius.titleT = QStringLiteral("Wt");
    beamRadius.titleS = QStringLiteral("Ws");
    beamRadius.unit = _colUnits.value(COL_BEAMSIZE, AppSettings::instance().defaultUnitBeamRadius);

    // ColumnDef aperRatio;
    // aperRatio.id = COL_APER_RATIO;
    // aperRatio.titleT = QStringLiteral("At");
    // aperRatio.titleS = QStringLiteral("As");
    // aperRatio.unit = Z::Units::none();

    ColumnDef frontRadius;
    frontRadius.id = COL_WAVEFRONT;
    frontRadius.titleT = QStringLiteral("Rt");
    frontRadius.titleS = QStringLiteral("Rs");
    frontRadius.unit = _colUnits.value(COL_WAVEFRONT, AppSettings::instance().defaultUnitFrontRadius);

    ColumnDef halfAngle;
    halfAngle.id = COL_ANGLE;
    halfAngle.titleT = QStringLiteral("Vt");
    halfAngle.titleS = QStringLiteral("Vs");
    halfAngle.unit = _colUnits.value(COL_ANGLE, AppSettings::instance().defaultUnitAngle);

    return { beamRadius, /*aperRatio,*/ frontRadius, halfAngle };
}

QString CustomTableFunction::columnTitle(const ColumnId &id) const
{
    if (id == COL_BEAMSIZE)
        return qApp->tr("Beam radius", "Table function column");
    if (id == COL_BEAMSIZE)
        return qApp->tr("Aperture ratio", "Table function column");
    if (id == COL_WAVEFRONT)
        return qApp->tr("Wavefront ROC", "Table function column");
    if (id == COL_ANGLE)
        return qApp->tr("Half div. angle", "Table function column");
    return id;
}

int CustomTableFunction::columnCount() const
{
    return COL_COUNT;
}

QVector<Z::PointTS> CustomTableFunction::calculatePumpBeforeSchema(Element *elem)
{
    Q_UNUSED(elem)

    return {};
}

QVector<Z::PointTS> CustomTableFunction::calculateSinglePass(Element *elem, RoundTripCalculator* calc, double ior) const
{
    Q_UNUSED(elem)

    return {};
}

QVector<Z::PointTS> CustomTableFunction::calculateResonator(Element *elem, RoundTripCalculator *calc, double ior) const
{
    Q_UNUSED(elem)
    
    return {};
}

bool CustomTableFunction::prepare()
{
    _errorLog.clear();
    _errorLine = 0;

    PyRunner py;
    py.schema = schema();
    py.code = _code;
    py.funcNames = { "describe_columns" };
    py.printFunc = _printFunc;
    
    if (!py.load()) {
        setError(py.errorText());
        _errorLog = py.errorLog;
        _errorLine = py.errorLine;
        return false;
    }
    
    return true;
}
