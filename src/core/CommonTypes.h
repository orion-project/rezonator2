#ifndef Z_COMMON_TYPES_H
#define Z_COMMON_TYPES_H

#include <QString>
#include <QVector>

enum class TripType
{
    SW, ///< Standing wave system
    RR, ///< Ring resonator
    SP  ///< Single pass system
};

class TripTypeInfo
{
public:
    QString alias() const;
    QString toolTip() const;
    QString fullHeader() const;
    QString iconPath() const;
private:
    struct TripTypeInfoPrivate* _info;
    TripTypeInfo(struct TripTypeInfoPrivate* info): _info(info) {}
    friend class TripTypes;
};

class TripTypes
{
public:
    static const TripTypeInfo& info(TripType tripType);
    static const QVector<TripType>& all();
    static TripType find(const QString& alias, bool* ok = nullptr);
    static TripType find(int id, bool* ok = nullptr);

private:
    static const TripTypeInfo& SW();
    static const TripTypeInfo& RR();
    static const TripTypeInfo& SP();
};

/// How to calculate stability parameter from round-trip matrix
enum class StabilityCalcMode
{
    Normal, ///< P = (A + B)/2
    Squared ///< P = 1 - ((A + D)/2)^2
};


#endif // Z_COMMON_TYPES_H
