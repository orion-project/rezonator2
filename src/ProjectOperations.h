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

    static QString getSaveFileName(QWidget *parent);
    static QString getOpenFileName(QWidget *parent);

    /// Open example schema file.
    /// We have to have an intended command for opening of example file, because of
    /// running the application from AppImage (Linux) or application bundle (MacOS)
    /// user has no access to examples directory via regular open file dialog.
    static QString selectSchemaExample();


public slots:
    void newSchemaFile();
    void openSchemaFile();
    void openSchemaFile(const QString& fileName);
    bool saveSchemaFile();
    bool saveSchemaFile(const QString& fileName);
    bool saveSchemaFileAs();
    void saveSchemaFileCopy();
    void setupPump();
    void setupWavelength();
    void setupTripType();

signals:
    void protocolRequired();

private:
    QWidget* _parent;
    Schema* _schema;
    CalcManager* _calcManager;

    void writeProtocol(const Z::Report& report, const QString &message);
};

#endif // PROJECT_OPERATIONS_H
