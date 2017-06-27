#ifndef Z_PROJECT_H
#define Z_PROJECT_H

#include <QObject>

#include "core/Schema.h"

namespace Z {

class Project : public QObject
{
    Q_OBJECT

public:
    explicit Project(QObject *parent = 0);
    ~Project();

    Schema* schema() { return _schema; }

    const QString& fileName() const { return _fileName; }
    void setFileName(const QString& fileName) { _fileName = fileName; }

private:
    Schema* _schema;
    QString _fileName;
};

} // namespace Z

#endif // Z_PROJECT_H
