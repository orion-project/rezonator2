#include "SchemaReaderIni.h"

#include "../core/Schema.h"
#include "../core/Elements.h"
#include "../core/ElementsCatalog.h"

#include <QFile>
#include <QPointer>
#include <QSettings>
#include <QVariant>

using namespace Z;

namespace OldSchema {

Ori::Version maxVersion() { return Ori::Version(1, 2); }
Ori::Version minVersion() { return Ori::Version(1, 0); }

Z::Unit parseLinear(const QString& s, Z::Unit def)
{
    static QMap<QString, Z::Unit> map({{"A", Z::Units::Ao()},
                                       {"nm", Z::Units::nm()},
                                       {"mkm", Z::Units::mkm()},
                                       {"mm", Z::Units::mm()},
                                       {"cm", Z::Units::cm()},
                                       {"m", Z::Units::m()}});
    return map.contains(s)? map[s]: def;
}

Z::Unit parseAngular(const QString& s, Z::Unit def)
{
    static QMap<QString, Z::Unit> map({{"rad", Z::Units::rad()},
                                       {"deg", Z::Units::deg()},
                                       {"mrad", Z::Units::mrad()},
                                       {"min", Z::Units::amin()}});
    return map.contains(s)? map[s]: def;
}

TripType parseTripType(int s)
{
    switch (s)
    {
    case 1: return TripType::RR;
    case 2: return TripType::SP;
    }
    return TripType::SW;
}

QString parseElemType(const QString& oldType, const Ori::Version& version)
{
    static QMap<QString, QString> map({
          {"TElemFlatMirror",        ElemFlatMirror::_type_() },
          {"TElemCurveMirror",       ElemCurveMirror::_type_() },
          {"TElemEmptyRange",        ElemEmptyRange::_type_() },
          {"TElemMediaRange",        ElemMediumRange::_type_() },
          {"TElemPlate",             ElemPlate::_type_() },
          {"TElemBrewsterCrystal",   ElemBrewsterCrystal::_type_() },
          {"TElemBrewsterPlate",     ElemBrewsterPlate::_type_() },
          {"TElemTiltedCrystal",     ElemTiltedCrystal::_type_() },
          {"TElemTiltedPlate",       ElemTiltedPlate::_type_() },
          {"TElemThinLens",          ElemThinLens::_type_() },
          {"TElemThinCylinderLensT", ElemCylinderLensT::_type_() },
          {"TElemThinCylinderLensS", ElemCylinderLensS::_type_() },
          {"TElemMatrix",            ElemMatrix::_type_() },
          {"TElemPoint",             ElemPoint::_type_() },
          {"TElemGrinLens",          ElemGrinLens::_type_()}
      });

    auto type = oldType;
    if (version.match(1, 0))
    {
        if (type == "TElemRange") type = "TElemPlate";
        else if (type == "TElemThinLensCylS") type = "TElemThinCylinderLensS";
        else if (type == "TElemThinLensCylT") type = "TElemThinCylinderLensT";
    }
    else if (version.less(1, 2))
    {
        if (type == "TElemCustom") type = "TElemMatrix";
    }
    return map.contains(type)? map[type]: QString();
}

enum PumpMode { Waist, Front, Complex, InvComplex, RayVector, TwoSects };

} // namespace OldSchema

//------------------------------------------------------------------------------
//                             IniSection
//------------------------------------------------------------------------------

class IniSection
{
public:
    IniSection(QSettings* file, const QString& section) : _file(file)
    {
        if (_file->childGroups().contains(section))
        {
            _file->beginGroup(section);
            _group = _file->group();
        }
    }

    ~IniSection()
    {
        if (!_file) return;
        if (!_group.isEmpty() && _file->group() == _group) _file->endGroup();
    }

    /// Returns Unicode representation of string values stored in .SHE files.
    /// Because of those files have no any information about their code page,
    /// current system code page is used for convert from Ascii to Unicode.
    QString getString(const QString& key, const QVariant& def = QVariant())
    {
        auto s = _file->value(key, def).toString();
        return QString::fromLocal8Bit(s.toLatin1().data());
    }

    bool getBool(const QString& key, bool def = false)
    {
        return _file->value(key, def).toBool();
    }

    double getValue(const QString& key, double def, bool* ok = nullptr)
    {
        if (!_file->contains(key))
        {
            if (ok) *ok = false;
            return def;
        }
        return _file->value(key, def).toDouble(ok);
    }

    int getInt(const QString& key, int def, bool* ok = nullptr)
    {
        if (!_file->contains(key))
        {
            if (ok) *ok = false;
            return def;
        }
        return _file->value(key, def).toInt(ok);
    }

    QString getText(const QString& key, const QVariant& def = QVariant())
    {
        QStringList targetLines;
        QStringList sourceLines = _file->value(key, def).toStringList();
        for (const QString& line: sourceLines)
            targetLines << QString::fromLocal8Bit(line.toLatin1().data());
        return targetLines.join('\n');
    }

    bool opened() const { return !_group.isEmpty(); }

private:
    QPointer<QSettings> _file;
    QString _group;
};

//------------------------------------------------------------------------------
//                             SchemaReaderIni
//------------------------------------------------------------------------------

void SchemaReaderIni::readFromFile(const QString &fileName)
{
    // QSettings creates a new file if it does't exists, so we should check it first
    if (!QFile::exists(fileName))
        return _report.error(QString("File not found: %1").arg(fileName));
    _file = new QSettings(fileName, QSettings::IniFormat);

    IniSection ini(_file, "PREFERENCES");
    if (!ini.opened())
        return _report.error(QString("Invalid file: main section not found"));

    _version = Ori::Version(ini.getString("Version"));
    if (_version < OldSchema::minVersion() || _version > OldSchema::maxVersion())
        return _report.error(QString(
            "File version %1 is not supported, supported versions: %2 - %3")
                .arg(_version.str(), OldSchema::minVersion().str(), OldSchema::maxVersion().str()));

    readGeneral(ini);
    readUnits(ini);
    readLambda(ini);
    readPump(ini);
    readElements();
    readWindows();

    delete _file;
}

void SchemaReaderIni::readGeneral(IniSection& ini)
{
    _schema->setNotes(ini.getText("Notes"));
    if (ini.getBool("HasTitle"))
        _schema->setTitle(ini.getString("Title"));
    _schema->setTripType(OldSchema::parseTripType(ini.getInt("SchemaKind", 0)));
    // TODO:? load calculation precision
}

void SchemaReaderIni::readLambda(IniSection& ini)
{
    bool ok;
    auto value = ini.getValue("Wavelen", 0, &ok);
    if (ok && value > 0)
        _schema->wavelength().setValue(Z::Value(value, _lambdaUnit));
    else
        _report.warning(QString("Invalid value for wavelength is stored in file."));
}

void SchemaReaderIni::readUnits(IniSection& ini)
{
    // Units section is inside of PREFERENCES section, so it should be opened before calling this func

    // There are only two linear units in V1 - mm and mkm
    // Wavelength is always nm, and beamsize is always mkm
    if (_version.match(1, 0))
    {
        _linearUnit = (ini.getInt("LinearUnits", 0)) == 0 ? Z::Units::mm() : Z::Units::mkm();
        switch (ini.getInt("AngleUnits", 0))
        {
        case 1: _angularUnit = Z::Units::deg(); break;
        case 2: _angularUnit = Z::Units::mrad(); break;
        case 3: _angularUnit = Z::Units::amin(); break;
        default:_angularUnit = Z::Units::rad(); break;
        }
        _beamsizeUnit = Z::Units::mkm();
        _lambdaUnit = Z::Units::nm();
    }
    else
    {
         IniSection section(_file, "Units");
        _linearUnit = OldSchema::parseLinear(section.getString("Linear"), Z::Units::mm());
        _angularUnit = OldSchema::parseAngular(section.getString("Angle"), Z::Units::deg());
        _beamsizeUnit = OldSchema::parseLinear(section.getString("Beamsize"), Z::Units::mkm());
        _lambdaUnit = OldSchema::parseLinear(section.getString("Lambda"), Z::Units::nm());
    }
}

void SchemaReaderIni::readPump(IniSection& ini)
{
    if (!_schema->isSP()) return;

    PumpParams *pump = nullptr;
    if (_version.match(1, 0))
    {
        // There are only two pump types in version 1:
        // PumpType = 0 - gauss, 1 - hypergauss
        // PumpSize - beamsize in mkm (circular beam)
        // PumpParam1 - M^2 for hypergauss
        PumpParams_Waist *p = new PumpParams_Waist;
        p->waist()->setValue(ValueTS(ini.getValue("PumpSize", 100), Units::mkm()));
        p->distance()->setValue(ValueTS(0, Units::mm()));
        if (ini.getInt("PumpType", 0) == 1)
            p->MI()->setValue(ValueTS(ini.getValue("PumpParam1", 1)));
        pump = p;
    }
    else
    {
        bool ok;
        IniSection params(_file, "Pump");
        int mode = params.getInt("ParamsKind", OldSchema::Waist, &ok);
        if (!ok)
            return _report.error("Schema is SP, but no pump is stored in the file");

        bool ok1, ok2, ok3, ok4, ok5, ok6;
        double param1T = params.getValue("Param1T", 100, &ok1);
        double param1S = params.getValue("Param1S", 100, &ok2);
        double param2T = params.getValue("Param2T", 0, &ok3);
        double param2S = params.getValue("Param2S", 0, &ok4);
        double param3T = params.getValue("QualT", 1, &ok5);
        double param3S = params.getValue("QualS", 1, &ok6);
        if (!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || !ok6)
            return _report.error("Invalid pump parameters are stored in the file");

        switch (mode)
        {
        case OldSchema::Waist:
            {
                PumpParams_Waist *p = new PumpParams_Waist;
                p->waist()->setValue(ValueTS(param1T, param1S, _beamsizeUnit));
                p->distance()->setValue(ValueTS(param2T, param2S, _linearUnit));
                p->MI()->setValue(ValueTS(param3T, param3S));
                pump = p;
                break;
            }
        case OldSchema::Front:
            {
                PumpParams_Front *p = new PumpParams_Front;
                p->beamRadius()->setValue(ValueTS(param1T, param1S, _beamsizeUnit));
                p->frontRadius()->setValue(ValueTS(param2T, param2S, _linearUnit));
                p->MI()->setValue(ValueTS(param3T, param3S));
                pump = p;
                break;
            }
        case OldSchema::Complex:
            {
                PumpParams_Complex *p = new PumpParams_Complex;
                p->real()->setValue(ValueTS(param1T, param1S, _linearUnit));
                p->imag()->setValue(ValueTS(param2T, param2S, _linearUnit));
                p->MI()->setValue(ValueTS(param3T, param3S));
                pump = p;
                break;
            }
        case OldSchema::InvComplex:
            {
                PumpParams_InvComplex *p = new PumpParams_InvComplex;
                p->real()->setValue(ValueTS(param1T, param1S, _linearUnit));
                p->imag()->setValue(ValueTS(param2T, param2S, _linearUnit));
                p->MI()->setValue(ValueTS(param3T, param3S));
                pump = p;
                break;
            }
        case OldSchema::RayVector:
            {
                PumpParams_RayVector *p = new PumpParams_RayVector;
                p->radius()->setValue(ValueTS(param1T, param1S, _beamsizeUnit));
                param2T = _angularUnit->fromSi(param2T); // It is stored in rads
                param2S = _angularUnit->fromSi(param2S); // It is stored in rads
                p->angle()->setValue(ValueTS(param2T, param2S, _angularUnit));
                p->distance()->setValue(ValueTS(0, _linearUnit)); // not used
                pump = p;
                break;
            }
        case OldSchema::TwoSects:
            {
                PumpParams_TwoSections *p = new PumpParams_TwoSections;
                p->radius1()->setValue(ValueTS(param1T, param1S, _beamsizeUnit));
                p->radius2()->setValue(ValueTS(param2T, param2S, _beamsizeUnit));
                p->distance()->setValue(ValueTS(param3T, param3S, _linearUnit));
                pump = p;
                break;
            }
        }
    }
    if (!pump)
        return _report.error("Schema is SP, but no pump is stored in the file");
    pump->activate(true);
    _schema->pumps()->append(pump);
}

void SchemaReaderIni::readElements()
{
    _file->endGroup();

    int maxElemIndex = -1;
    QStringList sections = _file->childGroups();
    // Sections are sorted like this: Element0, Element1, Element10, Element2 ...
    // So we can't read childGroups sequentially, calculate max instead
    for (const QString& section : sections)
        if (section.startsWith("Element"))
        {
            bool ok;
            int index = section.right(section.count()-7).toInt(&ok);
            if (ok && index > maxElemIndex) maxElemIndex = index;
        }
    for (int index = 0; index <= maxElemIndex; index++)
        readElement(QString("Element%1").arg(index));
}

void SchemaReaderIni::readElement(const QString &section)
{
    IniSection ini(_file, section);
    if (!ini.opened())
    {
        _report.warning(QString(
            "Section [%1] not found, some elements may not be loaded").arg(section));
        return;
    }

    auto oldType = ini.getString("Type", "NULL");
    auto newType = OldSchema::parseElemType(oldType, _version);
    auto elem = ElementsCatalog::instance().create(newType);
    if (!elem)
        return _report.warning(qApp->translate("IO",
            "Unknown element type '%1', element skipped").arg(oldType));

    _schema->insertElements({elem}, -1, Arg::RaiseEvents(false));

    ElementEventsLocker lock(elem);

    elem->setLabel(ini.getString("Alias"));
    elem->setTitle(ini.getString("Title"));
    elem->setDisabled(ini.getBool("Disabled"));

    auto grinLens = dynamic_cast<ElemGrinLens*>(elem);
    auto matrixElem = dynamic_cast<ElemMatrix*>(elem);

    for (Z::Parameter* param : elem->params())
    {
        auto paramAlias = param->alias();

        if (grinLens and param == grinLens->paramIor())
            paramAlias = "n0";

        bool ok;
        double tmp = ini.getValue("Param_" + paramAlias, 0, &ok);
        if (!ok)
        {
            // IOR is not stored in old schemas for empty ranges so there is no issue if that
            auto emptyRange = dynamic_cast<ElemEmptyRange*>(elem);
            if (!emptyRange || param != emptyRange->paramIor())
                _report.warning(QString("Invalid value for parameter %1 of element %2 "
                                        "is stored in file, default value is used")
                                        .arg(param->alias(), elem->displayLabel()));
            continue;
        }

        auto unit = paramUnit(param);
        // Angles are always stored in rads in old schemas, but are displayed in selected units.
        // Now they also must be stored in selected units.
        if (param->dim() == Z::Dims::angular())
        {
            tmp = unit->toSi(tmp);
            unit = _angularUnit;
            tmp = unit->fromSi(tmp);
        }

        // Gradiens are stored in linear units in old schemas, but they are in 1/m2 in new ones
        if (grinLens and (param == grinLens->paramIor2t() or param == grinLens->paramIor2s()))
            tmp /= _linearUnit->toSi(1) * _linearUnit->toSi(1);

        // C factors are stored in linear units in old schemas, but they are in 1/m in new ones
        if (matrixElem and (param == matrixElem->paramCt() or param == matrixElem->paramCs()))
            tmp /= _linearUnit->toSi(1);

        Z::Value value(tmp, unit);

        auto res = param->verify(value);
        if (!res.isEmpty())
        {
            _report.warning(QString(
                "Value %1 is unacceptable for parameter %2 of element %3: %4")
                    .arg(value.str(), param->alias(), elem->displayLabel(), res));
            continue;
        }

        param->setValue(value);
    }

    // TODO:NEXT-VER load misalignments
}

Z::Unit SchemaReaderIni::paramUnit(Z::Parameter* param) const
{
    if (param->dim() == Z::Dims::linear())
        return _linearUnit;

    // Angles are always stored in rads in old schemas
    if (param->dim() == Z::Dims::angular())
        return Z::Units::rad();

    if (param->dim() == Z::Dims::fixed())
        return param->value().unit();

    return Z::Units::none();
}

void SchemaReaderIni::readWindows()
{
    // TODO
}
