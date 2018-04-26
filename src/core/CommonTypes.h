#ifndef Z_COMMON_TYPES_H
#define Z_COMMON_TYPES_H

#include <QString>
#include <QVector>
#include <QObject>

namespace Z {
enum WorkPlane { Plane_T, Plane_S };
}

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

namespace Z {

/**
    Commonly used enums.
*/
class Enums
{
    Q_GADGET

public:
    /**
        How to calculate stability parameter from round-trip matrix.
    */
    enum StabilityCalcMode
    {
        Normal, ///< P = (A + B)/2
        Squared ///< P = 1 - ((A + D)/2)^2
    };
    Q_ENUM(StabilityCalcMode)

    Enums() = delete;
};


/**
    Helper class holding a result value of some operation or an error message if operation is failed.
*/
template <typename TResult> class Result
{
public:
    static Result success(TResult&& value)
    {
        Result r;
        r._value = value;
        return r;
    }

    static Result fail(const QString& error)
    {
        Result r;
        r._error = error;
        return r;
    }

    bool ok() const { return _error.isEmpty(); }
    TResult value() const { return _value; }
    QString error() const { return _error; }

private:
    Result() {}

    TResult _value;
    QString _error;
};

} // namespace Z

#endif // Z_COMMON_TYPES_H
