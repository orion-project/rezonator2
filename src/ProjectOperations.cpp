#include "ProjectOperations.h"

#include "AppSettings.h"
#include "CalcManager.h"
#include "CommonData.h"
#include "CustomPrefs.h"
#include "PumpParamsDialog.h"
#include "SchemaPropsDialog.h"
#include "core/Schema.h"
#include "core/Protocol.h"
#include "io/SchemaReaderIni.h"
#include "io/SchemaReaderJson.h"
#include "io/SchemaWriterJson.h"
#include "io/CommonUtils.h"

#include "helpers/OriDialogs.h"
#include "tools/OriWaitCursor.h"
#include "widgets/ParamEditor.h"
#include "widgets/OriOptionsGroup.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QTextStream>
#include <QListWidget>

QFileDialog::Options fileDialogOptions()
{
    QFileDialog::Options options = nullptr;
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

QString ProjectOperations::getOpenFileName(QWidget* parent)
{
    QString recentPath = CustomPrefs::recentDir("schema_open_path");
    QString recentFilter = CustomPrefs::recentStr("schema_open_filter");

    auto fileName = QFileDialog::getOpenFileName(parent,
                                                 tr("Open schema", "Dialog title"),
                                                 recentPath,
                                                 Z::IO::Utils::filtersForOpen(),
                                                 &recentFilter,
                                                 fileDialogOptions());
    if (fileName.isEmpty()) return QString();

    CustomPrefs::setRecentDir("schema_open_path", fileName);
    CustomPrefs::setRecentStr("schema_open_filter", recentFilter);

    fileName = Z::IO::Utils::refineFileName(fileName, recentFilter);

    CommonData::instance()->addFileToMruList(fileName);
    return fileName;
}

QString ProjectOperations::getSaveFileName(QWidget* parent)
{
    QString recentPath = CustomPrefs::recentDir("schema_save_path");
    QString recentFilter = CustomPrefs::recentStr("schema_save_filter");

    auto fileName = QFileDialog::getSaveFileName(parent,
                                                 tr("Save Schema", "Dialog title"),
                                                 recentPath,
                                                 Z::IO::Utils::filtersForSave(),
                                                 &recentFilter,
                                                 fileDialogOptions());
    if (fileName.isEmpty()) return QString();

    CustomPrefs::setRecentDir("schema_save_path", fileName);
    CustomPrefs::setRecentStr("schema_save_filter", recentFilter);

    fileName = Z::IO::Utils::refineFileName(fileName, recentFilter);

    CommonData::instance()->addFileToMruList(fileName);
    return fileName;
}

void ProjectOperations::openExampleFile(const QString& fileName)
{
    OpenFileOptions opts;
    opts.isExample = true;
    openSchemaFile(fileName, opts);
}

void ProjectOperations::openSchemaFile()
{
    auto fileName = getOpenFileName(_parent);
    if (fileName.isEmpty()) return;
    openSchemaFile(fileName);
}

void ProjectOperations::openSchemaFile(const QString& fileName)
{
    openSchemaFile(fileName, OpenFileOptions());
}

void ProjectOperations::openSchemaFile(const QString& fileName, const OpenFileOptions& opts)
{
    if (!schema()->state().isNew())
    {
        QStringList args;
        args << qApp->applicationFilePath();
        if (opts.isExample)
            args << "--example";
        args << " \"" + fileName + "\"";
        QProcess::startDetached(args.join(' '));
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

    if (!report.isEmpty())
        writeProtocol(report, tr("There are messages while loading project."));

    if (opts.isExample)
    {
        QFileInfo fileInfo(fileName);
        schema()->setTitle(fileInfo.baseName());
        // Don't set file name as we don't want to overwrite examples,
        // and we don't want to show unexpected paths
        // somewhere in /tmp if the program is launched from AppImage
        // or somewhere inside of application bundle if we are on MacOS
    }
    else
        schema()->setFileName(fileName);
    schema()->events().enable();
    schema()->events().raise(SchemaEvents::Loaded);
    schema()->events().raise(SchemaEvents::RecalRequred);
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
    if (fileName.isEmpty())
        return saveSchemaFileAs();
    if (Z::IO::Utils::isOldSchema(fileName))
        return Ori::Dlg::yes(tr("The schema is of old format and can not be saved, "
                                "would you like to save it as a file in the new format?"))
            && saveSchemaFileAs();
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
    if (!writer.report().isEmpty())
        writeProtocol(writer.report(), tr("There are messages while saving project."));

    if (!writer.report().hasErrors())
    {
        schema()->setFileName(fileName);
        schema()->events().raise(SchemaEvents::Saved);
    }

    return !writer.report().hasErrors();
}

bool ProjectOperations::saveSchemaFileAs()
{
    auto fileName = getSaveFileName(_parent);
    if (fileName.isEmpty()) return false;
    bool ok = saveSchemaFile(fileName);
    if (ok)
        CommonData::instance()->addFileToMruList(fileName);
    return ok;
}

void ProjectOperations::saveSchemaFileCopy()
{
    auto fileName = getSaveFileName(_parent);
    if (fileName.isEmpty()) return;

    Z_REPORT("Saving copy" << fileName)

    Ori::WaitCursor wc;

    SchemaWriterJson writer(schema());
    writer.writeToFile(fileName);

    if (!writer.report().isEmpty())
        writeProtocol(writer.report(), tr("There are messages while saving copy of project."));
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
        {
            schema()->events().raise(SchemaEvents::PumpChanged, pump);
            schema()->events().raise(SchemaEvents::RecalRequred);
        }
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

QString ProjectOperations::selectSchemaExample()
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

    QString fileName;

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
            fileName = examplesDir % '/' % selected->text();
    }

    return fileName;
}

void ProjectOperations::editSchemaProps()
{
   Z::Dlg::editSchemaProps(_parent, schema());
}
