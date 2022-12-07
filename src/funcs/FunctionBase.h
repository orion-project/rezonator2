#ifndef FUNCTION_BASE_H
#define FUNCTION_BASE_H

#include "../core/Element.h"
#include "../core/Variable.h"

#include "core/OriTemplates.h"

#define FUNC_NAME(s)\
    QString name() const override { return s; }

#define FUNC_ALIAS(s)\
    static QString _alias_() { return s; }\
    QString alias() const override { return s; }

#define FUNC_ICON(s)\
    const char* iconPath() const override { return s; }

class FunctionBase;
class RoundTripCalculator;
class Schema;

class FunctionListener
{
public:
    virtual ~FunctionListener();
    virtual void functionCalculated(FunctionBase*) {}
    virtual void functionDeleted(FunctionBase*) {}
};

/**
    Base class for all functions.
*/
class FunctionBase : public Ori::Notifier<FunctionListener>
{
public:
    enum FunctionState
    {
        Ok, ///< Function result is valid and actual.
        Frozen, ///< Function result has no relations to current schema anymore but valid.
        Dead, ///< Function result has no meaning anymore. Function window should be closed.
    };
public:
    virtual ~FunctionBase();

    Schema* schema() const { return _schema; }

    /// Returns display name of function.
    virtual QString name() const = 0;

    /// Function alias is used for saving/restoring of function windows.
    virtual QString alias() const { return QString(); }

    virtual QString helpTopic() const { return QString(); }

protected:
    Schema *_schema;

    FunctionBase(Schema *schema) : _schema(schema) {}
};

struct InfoFuncAction
{
    QString title;
    QString icon;
    std::function<void()> triggered;
    std::function<bool()> isChecked;
};

/**
    Base class for all information functions.
    Information function is a function presenting its calculation
    results in textual form. These results is generally displayed
    in special information popup window.
*/
class InfoFunction : public FunctionBase
{
public:
    virtual FunctionState elementDeleting(Element*);
    const QString& result() const { return _result; }
    void calculate();
    void freeze(bool on);
    bool frozen() const { return _frozen; }
    const QVector<InfoFuncAction>& actions() const { return _actions; }

protected:
    InfoFunction(Schema *schema) : FunctionBase(schema) {}
    virtual QString calculateInternal() { return QString(); }

    QVector<InfoFuncAction> _actions;
private:
    QString _result;
    bool _frozen = false;
    bool _needRecalc = false;
};

#endif // FUNCTION_BASE_H
