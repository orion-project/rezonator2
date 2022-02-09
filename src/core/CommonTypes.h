#ifndef Z_COMMON_TYPES_H
#define Z_COMMON_TYPES_H

#include <QMetaEnum>
#include <QString>
#include <QVector>
#include <QObject>

#include <complex>

namespace Z {
enum WorkPlane { Plane_T, Plane_S };

typedef std::complex<double> Complex;
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
        Normal, ///< P = (A + D)/2
        Squared ///< P = 1 - ((A + D)/2)^2
    };
    Q_ENUM(StabilityCalcMode)

    Enums() = delete;

    template <typename TEnum>
    static QString toStr(TEnum value)
    {
        return QMetaEnum::fromType<TEnum>().key(value);
    }

    template <typename TEnum>
    static TEnum fromStr(const QString& value, TEnum defaultValue)
    {
        bool ok;
        int res = QMetaEnum::fromType<TEnum>().keyToValue(value.toLatin1().data(), &ok);
        return ok ? TEnum(res) : defaultValue;
    }

    static QString displayStr(StabilityCalcMode mode);
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
        r._value = std::move(value);
        return r;
    }

    static Result success(const TResult& value)
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
