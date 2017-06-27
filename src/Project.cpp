#include "Project.h"

namespace Z {

Project::Project(QObject *parent) : QObject(parent)
{
    _schema = new Schema;
}

Project::~Project()
{
    delete _schema;
}

} // namespace Z
