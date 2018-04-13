#include "SchemaReaderIni.h"

#include "../core/Schema.h"
#include "../core/Elements.h"
#include "../core/ElementsCatalog.h"
// TODO:NEXT-VER #include "../core/Pump.h"

#include <QFile>
#include <QSettings>
#include <QVariant>

namespace OldSchema {

Ori::Version maxVersion() { return Ori::Version(1, 2); }
Ori::Version minVersion() { return Ori::Version(1, 1); }

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
      });
    // TODO:NEXT-VER load grin-lens

    auto type = oldType;
    if (version.less(1, 2))
    {
        if (type == "TElemCustom") type = "TElemMatrix";
    }
    return map.contains(type)? map[type]: QString();
}

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

    double getValue(const QString& key, double def, bool* ok = 0)
    {
        if (!_file->contains(key))
        {
            if (ok) *ok = false;
            return def;
        }
        return _file->value(key, def).toDouble(ok);
    }

    int getInt(const QString& key, int def, bool* ok = 0)
    {
        if (!_file->contains(key))
        {
            if (ok) *ok = false;
            return def;
        }
        return _file->value(key, def).toInt(ok);
    }

    bool opened() const { return !_group.isEmpty(); }

private:
    QSettings* _file;
    QString _group;
};

//------------------------------------------------------------------------------
//                             SchemaReaderIni
//------------------------------------------------------------------------------

void SchemaReaderIni::readFromFile(const QString &fileName)
{
    // QSettings creates a new file if it does't exists, so we should check it first
    if (!QFile::exists(fileName))
        return _report.error(qApp->translate("IO", "File not found: %1").arg(fileName));
    _file = new QSettings(fileName, QSettings::IniFormat);

    IniSection ini(_file, "PREFERENCES");
    if (!ini.opened())
        return _report.error(qApp->translate("IO", "Invalid file: main section not found"));

    _version = Ori::Version(ini.getString("Version"));
    if (_version < OldSchema::minVersion() || _version > OldSchema::maxVersion())
        return _report.error(qApp->translate("IO",
            "File version %1 is not supported, supported versions: %2 - %3")
                .arg(_version.str(), OldSchema::minVersion().str(), OldSchema::maxVersion().str()));

    readGeneral(ini);
    readUnits();
    readLambda(ini);
    readPump(ini);
    readElements();
    readWindows();

    delete _file;
}

void SchemaReaderIni::readGeneral(IniSection& ini)
{
    // TODO:? _schema->setComment(ini.getString("Notes"));
    // TODO:? _schema->setTitle(ini.getString("Title"));
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
        _report.warning(qApp->translate("IO", "Invalid value for wavelength is stored in file."));
}

void SchemaReaderIni::readUnits()
{
    // Units section is inside of PREFERENCES section, so it should be opened before calling this func
    IniSection section(_file, "Units");
    _linearUnit = OldSchema::parseLinear(section.getString("Linear"), Z::Units::mm());
    _angularUnit = OldSchema::parseAngular(section.getString("Angle"), Z::Units::deg());
    _beamsizeUnit = OldSchema::parseLinear(section.getString("Beamsize"), Z::Units::mkm());
    _lambdaUnit = OldSchema::parseLinear(section.getString("Lambda"), Z::Units::nm());
}

void SchemaReaderIni::readPump(IniSection& ini)
{
    Q_UNUSED(ini)

 /* TODO:NEXT-VER load pump from ini
    Z::Def::setDefaultPump(_schema);

    if (_schema->tripType() != Schema::SP) return;

    Z::Pump::Params pump;

    if (_version.match(1, 0))
    {
        // There are only two pump types in version 1:
        // PumpType = 0 - gauss, 1 - hypergauss
        // PumpSize - beamsize in mkm (circular beam)
        // PumpParam1 - M^2 for hypergauss
        auto size = ini.getValue("PumpSize", 100);
        pump.mode = Z::Pump::PumpMode_waist;
        pump.waist.distance = 0;
        pump.waist.radius = Z::Units::convert(size, Z::Units::mkm, _schema->unitsBeamsize());
        pump.waist.mi = ini.getInt("PumpType", 0) == 1? ini.getValue("PumpParam1", 1): 1;
    }
    else
    {
        bool ok1, ok2, ok3, ok4, ok5, ok6;
        IniSection params(_file, "Pump");
        int mode = params.getInt("ParamsKind", 0, &ok1);
        if (!ok1)
        {
            report(Issue::NoPump);
            return;
        }
        switch (mode)
        {
        case 0: pump.mode = Z::Pump::PumpMode_waist; break;
        case 1: pump.mode = Z::Pump::PumpMode_front; break;
        case 2: pump.mode = Z::Pump::PumpMode_complex; break;
        case 3: pump.mode = Z::Pump::PumpMode_icomplex; break;
        case 4: pump.mode = Z::Pump::PumpMode_vector; break;
        case 5: pump.mode = Z::Pump::PumpMode_sections; break;
        default:
            report(Issue::UnknownPumpMode, QString::number(mode));
            return;
        }
        Z::Pump::Params::Raw *raw = pump.params();
        raw->param1.T = params.getValue("Param1T", 100, &ok1);
        raw->param1.S = params.getValue("Param1S", 100, &ok2);
        raw->param2.T = params.getValue("Param2T", 0, &ok3);
        raw->param2.S = params.getValue("Param2S", 0, &ok4);
        raw->param3.T = params.getValue("QualT", 1, &ok5);
        raw->param3.S = params.getValue("QualS", 1, &ok6);
        if (!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || !ok6)
        {
            report(Issue::BadPumpParams);
            setDefaultPump(_schema);
            return;
        }
        if (pump.mode == Z::PumpMode_vector)
        {
            // old schemas always store angles in rad
            raw->param2.T = Z::Units::convert(raw->param2.T, Z::Units::rad, _schema->unitsAngle());
            raw->param2.S = Z::Units::convert(raw->param2.S, Z::Units::rad, _schema->unitsAngle());
            raw->param3 = 1;
        }
    }
    _schema->setPumpParams(pump);*/
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
        _report.warning(qApp->translate("IO",
            "Section [%1] not found, some elements may not be loaded").arg(section));
        return;
    }

    auto oldType = ini.getString("Type", "NULL");
    auto newType = OldSchema::parseElemType(oldType, _version);
    auto elem = ElementsCatalog::instance().create(newType);
    if (!elem)
        return _report.warning(qApp->translate("IO",
            "Unknown element type '%1', element skipped").arg(oldType));

    _schema->insertElement(elem, -1, false);

    ElementLocker lock(elem);

    elem->setLabel(ini.getString("Alias"));
    elem->setTitle(ini.getString("Title"));
    elem->setDisabled(ini.getBool("Disabled"));

    for (Z::Parameter* param : elem->params())
    {
        bool ok;
        double tmp = ini.getValue("Param_" + param->alias(), 0, &ok);
        if (!ok)
        {
            // IOR does not stored in old schemas for empty ranges
            _report.warning(qApp->translate("IO",
                "Invalid value for parameter %1 is stored in file, default value is used").arg(param->alias()));
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
        Z::Value value(tmp, unit);

        auto res = param->verify(value);
        if (!res.isEmpty())
        {
            // TODO:TEST make test to show this message when validation will be
            _report.warning(qApp->translate("IO",
                "Value %1 is unacceptable for parameter %2: %3").arg(value.str()).arg(param->alias()).arg(res));
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

    return Z::Units::none();
}

void SchemaReaderIni::readWindows()
{
    // TODO
}
