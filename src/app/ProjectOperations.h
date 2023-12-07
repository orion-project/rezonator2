#ifndef PROJECT_OPERATIONS_H
#define PROJECT_OPERATIONS_H

#include <QObject>

#include "../core/CommonTypes.h"

class CalcManager;
class Schema;
class PumpParams;

namespace Z {
    class Report;
}
namespace Ori {
    class MruList;
    class Settings;
}

class ProjectOperations : public QObject
{
    Q_OBJECT

public:
    struct OpenFileOptions {
        bool isExample = false;
        bool addToMru = false;
    };

    struct SaveFileOptions {
        bool asCopy = false;
        bool addToMru = false;
    };

public:
    explicit ProjectOperations(Schema *schema, QWidget *parent, CalcManager *calcManager, Ori::MruList* mruList);
    Schema* schema() const { return _schema; }
    bool canClose();

    static QString getSaveFileName(QWidget *parent);
    static QString getOpenFileName(QWidget *parent);

    /// Open example schema file.
    /// We have to have an intended command for opening of example file, because of
    /// running the application from AppImage (Linux) or application bundle (MacOS)
    /// user has no access to examples directory via regular open file dialog.
    static QString selectSchemaExample();

    static Schema* createDefaultSchema(TripType tripType);
    static void createDefaultPump(Schema* schema);

    void openSchemaFile(const QString& fileName, const OpenFileOptions& opts);
    bool saveSchemaFile(const QString& fileName, const SaveFileOptions& opts);

public slots:
    void newSchemaFile();
    void openSchemaFile();
    void openExampleFile();
    bool saveSchemaFile();
    bool saveSchemaFileAs();
    void saveSchemaFileCopy();
    void setupPump();
    void setupWavelength();
    void setupTripType();
    void editSchemaProps();

signals:
    void protocolRequired();

protected:
    virtual bool editPumpDlg(PumpParams* pump);
    virtual bool selectTripTypeDlg(TripType* tripType);

private:
    QWidget* _parent;
    Schema* _schema;
    CalcManager* _calcManager;
    Ori::MruList* _mruList;

    void writeProtocol(const Z::Report& report, const QString &message);
};

#endif // PROJECT_OPERATIONS_H
