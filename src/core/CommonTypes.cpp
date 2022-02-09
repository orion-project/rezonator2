#include "CommonTypes.h"

#include <QApplication>
#include <QDebug>

namespace Z {

QString Enums::displayStr(StabilityCalcMode mode)
{
    switch (mode)
    {
    case Normal:
        return QStringLiteral("(A + D)/2");
    case Squared:
        return QStringLiteral("1 - ((A + D)/2)²");
    }
    return QString();
}

} // namespace Z

//------------------------------------------------------------------------------

struct TripTypeInfoPrivate {
    QString alias;
    const char* toolTip;
    const char* fullHeader;
    QString iconPath;
};

QString TripTypeInfo::alias() const { return _info->alias; }
QString TripTypeInfo::toolTip() const { return qApp->tr(_info->toolTip); }
QString TripTypeInfo::fullHeader() const { return qApp->tr(_info->fullHeader); }
QString TripTypeInfo::iconPath() const { return _info->iconPath; }

//------------------------------------------------------------------------------

const TripTypeInfo& TripTypes::SW() {
    static TripTypeInfoPrivate p {
        "SW",
        "Standing wave system (SW)",
        "SW - Standing wave system",
        ":/icons/trip_type_sw"
    };
    static TripTypeInfo t(&p);
    return t;
}

const TripTypeInfo& TripTypes::RR() {
    static TripTypeInfoPrivate p {
        "RR",
        "Ring resonator (RR)",
        "RR - Ring resonator",
        ":/icons/trip_type_rr"
    };
    static TripTypeInfo t(&p);
    return t;
}

const TripTypeInfo& TripTypes::SP() {
    static TripTypeInfoPrivate p {
        "SP",
        "Single pass system (SP)",
        "SP - Single-pass system",
        ":/icons/trip_type_sp"
    };
    static TripTypeInfo t(&p);
    return t;
}

const TripTypeInfo& TripTypes::info(TripType tripType) {
    switch (tripType) {
    case TripType::SW: return SW();
    case TripType::RR: return RR();
    case TripType::SP: return SP();
    }
    qWarning() << "No info is available for trip type" << int(tripType);
    static TripTypeInfoPrivate empty { "?", nullptr, nullptr, QString() };
    static TripTypeInfo unknown(&empty);
    return unknown;
}

const QVector<TripType>& TripTypes::all() {
    static QVector<TripType> list { TripType::SW, TripType::RR, TripType::SP };
    return list;
}

TripType TripTypes::find(const QString& alias, bool* ok) {
    if (ok) *ok = true;
    for (TripType tripType : all())
        if (info(tripType).alias() == alias)
            return tripType;
    qWarning() << "Unknown trip type alias" << alias << "SW used as fallback";
    if (ok) *ok = false;
    return TripType::SW;
}

TripType TripTypes::find(int id, bool* ok) {
    if (ok) *ok = true;
    for (TripType tripType : all())
        if (int(tripType) == id)
            return tripType;
    qWarning() << "Unknown trip type id" << id << "SW used as fallback";
    if (ok) *ok = false;
    return TripType::SW;
}

//------------------------------------------------------------------------------
