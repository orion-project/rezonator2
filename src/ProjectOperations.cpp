#include "AppSettings.h"
#include "CalcManager.h"
#include "CustomPrefs.h"
#include "ProjectOperations.h"
#include "PumpParamsDialog.h"
#include "io/z_io_utils.h"
#include "io/SchemaReaderIni.h"
#include "io/SchemaReaderJson.h"
#include "io/SchemaWriterJson.h"
#include "core/Schema.h"
#include "core/Protocol.h"
#include "widgets/ParamEditor.h"
#include "widgets/OriOptionsGroup.h"
#include "helpers/OriDialogs.h"
#include "tools/OriWaitCursor.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QTextStream>
#include <QListWidget>

QFileDialog::Options fileDialogOptions()
{
    QFileDialog::Options options = 0;
    if (!Settings::instance().useSystemDialogs)
        options |= QFileDialog::DontUseNativeDialog;
    return options;
}

//------------------------------------------------------------------------------

ProjectOperations::ProjectOperations(Schema *schema, QWidget *parent, CalcManager *calcManager) :
    QObject(parent), _parent(parent), _schema(schema), _calcManager(calcManager)
{
}

void ProjectOperations::newSchemaFile()
{
    QProcess::startDetached(qApp->applicationFilePath());
}

QString ProjectOperations::getOpenFileName()
{
    QString recentPath = CustomPrefs::recentDir("schema_open_path");
    QString recentFilter = CustomPrefs::recentStr("schema_open_filter");

    auto fileName = QFileDialog::getOpenFileName(_parent,
                                                 tr("Open schema", "Dialog title"),
                                                 recentPath,
                                                 Z::IO::Utils::filtersForOpen(),
                                                 &recentFilter,
                                                 fileDialogOptions());
    if (fileName.isEmpty()) return QString();

    CustomPrefs::setRecentDir("schema_open_path", fileName);
    CustomPrefs::setRecentStr("schema_open_filter", recentFilter);

    fileName = Z::IO::Utils::refineFileName(fileName, recentFilter);

    emit fileNameSelected(fileName);
    return fileName;
}

QString ProjectOperations::getSaveFileName()
{
    QString recentPath = CustomPrefs::recentDir("schema_save_path");
    QString recentFilter = CustomPrefs::recentStr("schema_save_filter");

    auto fileName = QFileDialog::getSaveFileName(_parent,
                                                 tr("Save Schema", "Dialog title"),
                                                 recentPath,
                                                 Z::IO::Utils::filtersForSave(),
                                                 &recentFilter,
                                                 fileDialogOptions());
    if (fileName.isEmpty()) return QString();

    CustomPrefs::setRecentDir("schema_save_path", fileName);
    CustomPrefs::setRecentStr("schema_save_filter", recentFilter);

    fileName = Z::IO::Utils::refineFileName(fileName, recentFilter);

    emit fileNameSelected(fileName);
    return fileName;
}

void ProjectOperations::openSchemaFile()
{
    auto fileName = getOpenFileName();
    if (fileName.isEmpty()) return;
    openSchemaFile(fileName);
}

void ProjectOperations::openSchemaFile(const QString& fileName)
{
    if (!schema()->state().isNew())
    {
        QProcess::startDetached(qApp->applicationFilePath() + " \"" + fileName + "\"");
        return;
    }

    Z_REPORT("Loading" << fileName)
    Z::Report report;

    schema()->events().raise(SchemaEvents::Loading);
    schema()->events().disable();

    if (Z::IO::Utils::isOldSchema(fileName))
    {
        Ori::WaitCursor wc;
        SchemaReaderIni reader(schema());
        reader.readFromFile(fileName);
        report = reader.report();
    }
    else
    {
        Ori::WaitCursor wc;
        SchemaReaderJson reader(schema());
        reader.readFromFile(fileName);
        report = reader.report();
    }

    if (!report.IsEmpty())
        writeProtocol(report, tr("There are messages while loading project."));

    schema()->setFileName(fileName);
    schema()->events().enable();
    schema()->events().raise(SchemaEvents::Loaded);
}

void ProjectOperations::writeProtocol(const Z::Report& report, const QString& message)
{
    emit protocolRequired();
    for (const Z::Report::Event& event: report.events())
    {
        QString m = event.message();
        switch (event.type())
        {
        case Z::Report::Event::Info: Z_INFO(m) break;
        case Z::Report::Event::Warning: Z_WARNING(m) break;
        case Z::Report::Event::Error: Z_ERROR(m) break;
        }
    }
    if (!message.isEmpty())
    {
        QString msg = message % "\n\n" % tr("See protocol window for details.");
        if (report.hasErrors())
            Ori::Dlg::error(msg);
        else if (report.hasWarnings())
            Ori::Dlg::warning(msg);
        else if (report.hasInfo())
            Ori::Dlg::info(msg);
    }
}

bool ProjectOperations::saveSchemaFile()
{
    auto fileName = schema()->fileName();
    if (fileName.isEmpty() || Z::IO::Utils::isOldSchema(fileName))
        return saveSchemaFileAs();
    return saveSchemaFile(fileName);
}

bool ProjectOperations::saveSchemaFile(const QString& fileName)
{
    Z_REPORT("Saving" << fileName)

    SchemaWriterJson writer(schema());
    {
        Ori::WaitCursor wc;
        writer.writeToFile(fileName);
    }
    if (!writer.report().IsEmpty())
        writeProtocol(writer.report(), tr("There are messages while saving project."));

    if (!writer.report().hasErrors())
    {
        schema()->setFileName(fileName);
        schema()->events().raise(SchemaEvents::Saved);
    }

    return writer.report().hasErrors();
}

bool ProjectOperations::saveSchemaFileAs()
{
    auto fileName = getSaveFileName();
    if (fileName.isEmpty()) return false;
    return saveSchemaFile(fileName);
}

void ProjectOperations::saveSchemaFileCopy()
{
    auto fileName = getSaveFileName();
    if (fileName.isEmpty()) return;

    Z_REPORT("Saving copy" << fileName)

    Ori::WaitCursor wc;

    SchemaWriterJson writer(schema());
    writer.writeToFile(fileName);

    if (!writer.report().IsEmpty())
        writeProtocol(writer.report(), tr("There are messages while saving copy of project."));
}

void ProjectOperations::checkCmdLine()
{
    QStringList args = QApplication::arguments();
    for (int i = 1; i < args.count(); i++)
        if (QFileInfo(args[i]).exists())
        {
            openSchemaFile(args[i]);
            break;
        }
}

bool ProjectOperations::canClose()
{
    if (schema()->modified())
    {
        if (schema()->fileName().isEmpty())
            switch (Ori::Dlg::yesNoCancel(tr("Schema has not been saved. Save it before closing?")))
            {
            case QMessageBox::Cancel: return false;
            case QMessageBox::Yes: return saveSchemaFileAs();
            default: return true;
            }
        else
            switch (Ori::Dlg::yesNoCancel(tr("%1\n\nSchema has been modified. Save changes?").arg(schema()->fileName())))
            {
            case QMessageBox::Cancel: return false;
            case QMessageBox::Yes: return saveSchemaFile();
            default: return true;
            }
    }
    return true;
}

bool createFirstPump(Schema* schema, bool raiseParamsChanged)
{
    auto pump = PumpParamsDialog::makeNewPump();
    if (!pump) return false;

    schema->pumps()->append(pump);
    schema->events().raise(SchemaEvents::PumpCreated, pump);

    pump->activate(true);
    if (raiseParamsChanged)
        schema->events().raise(SchemaEvents::ParamsChanged);

    return true;
}

void ProjectOperations::setupPump()
{
    if (!schema()->isSP()) return;

    if (schema()->pumps()->isEmpty())
    {
        const bool raiseParamsChanged = true;
        createFirstPump(schema(), raiseParamsChanged);
    }
    else
    {
        auto pump = schema()->activePump();
        if (PumpParamsDialog::editPump(pump))
            schema()->events().raise(SchemaEvents::PumpChanged, pump);
    }
}

void ProjectOperations::setupWavelength()
{
    ParamEditor editor(&schema()->wavelength());
    Ori::Dlg::Dialog(&editor)
            .withTitle(schema()->wavelength().name())
            .withHelpTopic("") // TODO help topic
            .withIconPath(":/window_icons/wavelength")
            .withHorizontalPrompt(tr("Enter new wavelength:"))
            .connectOkToContentApply()
            .exec();
}

void ProjectOperations::setupTripType()
{
    Ori::Widgets::OptionsGroup group(true);
    group.setTitle(tr("Round trip type"));

    group.addOption(int(TripType::SW), TripTypes::info(TripType::SW).fullHeader());
    group.addOption(int(TripType::RR), TripTypes::info(TripType::RR).fullHeader());
    group.addOption(int(TripType::SP), TripTypes::info(TripType::SP).fullHeader());

    group.setOption(int(schema()->tripType()));
    auto dlg = Ori::Dlg::Dialog(&group)
            .withHelpTopic("") // TODO help topic
            .withContentToButtonsSpacingFactor(2);
    if (dlg.exec())
    {
        auto tripType = TripTypes::find(group.option());
        if (tripType == TripType::SP && schema()->pumps()->isEmpty())
        {
            const bool raiseParamsChanged = false;
            // ParamsChanged will be raised from setTripType()
            if (!createFirstPump(schema(), raiseParamsChanged))
                return;
        }
        schema()->setTripType(tripType);
    }
}

void ProjectOperations::openSchemaExample()
{
    QString examplesDir = qApp->applicationDirPath() % "/examples";
    QStringList exampleFiles = QDir(examplesDir).entryList(QDir::Files, QDir::Name);
#ifdef Q_OS_MAC
    if (exampleFiles.isEmpty())
    {
        // Lock near the application bundle, it is for development mode
        examplesDir = qApp->applicationDirPath() % "/../../../examples";
        examplesDir = QDir(examplesDir).absolutePath();
        exampleFiles = QDir(examplesDir).entryList(QDir::Files, QDir::Name);
    }
#endif
    QListWidget fileList;
#ifdef Q_OS_WIN
    // Default icon size looks OK on Ubuntu and MacOS but it is too small on Windows
    fileList.setIconSize(QSize(24, 24));
#endif
    for (auto fileName : exampleFiles)
        fileList.addItem(new QListWidgetItem(QIcon(":/window_icons/schema"), fileName));

    Ori::Dlg::Dialog dlg(&fileList);
    dlg.withTitle(tr("Open Example Schema"))
       .withStretchedContent()
       .withInitialSize(CustomPrefs::recentSize("open_example_dlg_size"))
       .withOkSignal(SIGNAL(itemDoubleClicked(QListWidgetItem*)));
    if (dlg.exec())
    {
        CustomPrefs::setRecentSize("open_example_dlg_size", dlg.size());
        QListWidgetItem *selected = fileList.currentItem();
        if (selected)
            openSchemaFile(examplesDir % '/' % selected->text());
    }
}
