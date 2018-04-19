#ifndef FORMULA_H
#define FORMULA_H

#include "Parameters.h"

#include <QMap>

namespace Z {

class Formula : public ParameterListener
{
public:
    Formula(Parameter* target);
    virtual ~Formula();

    bool prepare(Parameters &availableDeps);
    void calculate();

    Parameter* target() { return _target; }
    const Z::Parameters& deps() { return _deps; }

    const QString& code() const { return _code; }
    void setCode(const QString& code) { _code = code; }

    bool ok() const { return _status.isEmpty(); }
    const QString& status() const { return _status; }

    void addDep(Parameter* param);
    void removeDep(Parameter* param);
    void assignDeps(const Formula *formula);

    void parameterChanged(ParameterBase*) override { calculate(); }

private:
    Parameter* _target;
    Parameters _deps;
    QString _code;
    QString _status;
};

//------------------------------------------------------------------------------

class Formulas
{
public:
    Formula* get(Parameter*);
    void put(Formula*);
    void free(Parameter*);
    void clear();

    QMap<Parameter*, Formula*>& items() { return _items; }

    /// Checks if `whichParam` depends on `onParam`.
    bool ifDependsOn(Parameter *whichParam, Parameter *onParam) const;

    /// Returns list of parameter which depends on specified parameter.
    Parameters dependentParams(Parameter *whichParam) const;

private:
    QMap<Parameter*, Formula*> _items;
};

//------------------------------------------------------------------------------

namespace FormulaUtils {

/// Checks if string is valid variable name for usage in formulas.
/// Validity criteria are same as for C++ variable names.
bool isValidVariableName(const QString& s);

} // namespace ParameterUtils

} // namespace Z

#endif // FORMULA_H
