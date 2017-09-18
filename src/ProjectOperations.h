#ifndef PROJECT_OPERATIONS_H
#define PROJECT_OPERATIONS_H

#include <QObject>

class Schema;
class CalcManager;

namespace Z {
    class Report;
}

class ProjectOperations : public QObject
{
    Q_OBJECT

public:
    explicit ProjectOperations(Schema *schema, QWidget *parent, CalcManager *calcManager);
    Schema* schema() const { return _schema; }
    bool canClose();

public slots:
    void newSchemaFile();
    void openSchemaFile();
    void openSchemaFile(const QString& fileName);
    bool saveSchemaFile();
    bool saveSchemaFile(const QString& fileName);
    bool saveSchemaFileAs();
    void saveSchemaFileCopy();
    void checkCmdLine();
    void setupSchema();
    void setupPump();
    void setupWavelength();
    void setupTripType();

signals:
    void fileNameSelected(const QString& fileName);
    void protocolRequired();

private:
    QWidget* _parent;
    Schema* _schema;
    CalcManager* _calcManager;

    QString getSaveFileName();
    QString getOpenFileName();

    QString saveText(const QString& text, const QString& fileName);

    void writeProtocol(const Z::Report& report, const QString &message);
};

#endif // PROJECT_OPERATIONS_H
