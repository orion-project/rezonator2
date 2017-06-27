#include "Calculator.h"
#include "FunctionBase.h"
#include "../core/Schema.h"
#include "../core/Protocol.h"

#include <QApplication>
#include <QSettings>

/*namespace FuncHelpers {

void openGroup(QSettings *settings, const QString& group)
{
    while (!settings->group().isEmpty())
        settings->endGroup();
    settings->beginGroup("State_" + group);
}

void loadVariable(QSettings *settings, Schema *schema, Z::Variable& arg, const Elements& elems)
{
    // init from selected element
    foreach (Element *elem, elems)
        if (elem->params().size() > 0 && elem->owner() == schema)
        {
            arg.element = elem->owner()->indexOf(elem);
            arg.param = 0;
            break;
        }
    // init from saved element
    if (arg.element < 0)
    {
        QString elemLabel = settings->value("Elem").toString();
        QString paramName = settings->value("Param").toString();
        for (int i = 0; i < schema->count(); i++)
        {
            Element *elem = schema->element(i);
            if (elem->displayLabel() == elemLabel)
            {
// TODO
//                arg.element = i;
//                for (int j = 0; j < elem->params().size(); j++)
//                    if (elem->params().at(j).name == paramName)
//                    {
//                        arg.param = j;
//                        break;
//                    }
                break;
            }
        }
    }
    // init from first acceptable element
    if (arg.element < 0)
        for (int i = 0; i < schema->count(); i++)
            if (schema->element(i)->params().size() > 0)
            {
                arg.element = i;
                arg.param = i;
                break;
            }
    arg.load(settings);
}

void saveVariable(QSettings *settings, Schema *schema, Z::Variable& arg)
{
// TODO
//    if (arg.element >= 0 && arg.element < schema->count())
//    {
//        Element *elem = schema->element(arg.element);
//        settings->setValue("Elem", elem->displayLabel());
//        if (arg.param >= 0 && arg.param < elem->params().size())
//            settings->setValue("Param", elem->params().at(arg.param).name);
//    }
//    arg.save(settings);
}

} // namespace FuncHelpers
*/

//------------------------------------------------------------------------------
//                                 FunctionBase
//------------------------------------------------------------------------------

FunctionBase::FunctionState InfoFunction::elementDeleting(Element*)
{
    return _schema->count() == 1? Dead: Ok; // last element is deleting
}


