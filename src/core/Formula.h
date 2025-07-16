#ifndef FORMULA_H
#define FORMULA_H

#include "Parameters.h"

#include <QMap>

namespace Z {

/**
    Formula can calculate an expression given as a string and assign the result to the target parameter.
    It can have other parameters as dependencies and use their names in the expression.
*/
class Formula : public ParameterListener
{
public:
    Formula(Parameter* target);
    ~Formula() override;

    bool prepare(Parameters &availableDeps);
    void calculate();

    Parameter* target() { return _target; }
    const Z::Parameters& deps() { return _deps; }

    const QString& code() const { return _code; }
    void setCode(const QString& code) { _code = code; }

    bool ok() const { return _error.isEmpty(); }
    const QString& error() const { return _error; }
    void setError(const QString &error);

    void addDep(Parameter* param);
    void removeDep(Parameter* param);
    void assignDeps(const Formula *formula);

    void parameterChanged(ParameterBase*) override { calculate(); }
    void parameterFailed(ParameterBase*) override { calculate(); }
    
    /// Put a new parameter name into the formula code if it depend on the param.
    /// Returns true if the parameter is found in the formula code.
    bool renameDependency(Parameter *param, const QString &newName);

    QString displayStr() const;

private:
    Parameter* _target;
    Parameters _deps;
    QString _code;
    QString _error;
};

//------------------------------------------------------------------------------

/**
    Contains a set of @a Z::Formula instances and their mappings to target parameters.
*/
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
    
    /// Put a new parameter name into formulas code if they depend on it.
    /// Returns true if the parameter is found in the code of a formula.
    bool renameDependency(Parameter *param, const QString &newName);

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
