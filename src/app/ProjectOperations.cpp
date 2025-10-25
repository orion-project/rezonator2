#include "ProjectOperations.h"

#include "../app/AppSettings.h"
#include "../app/CalcManager.h"
#include "../app/HelpSystem.h"
#include "../app/PersistentState.h"
#include "../app/HelpSystem.h"
#include "../core/Schema.h"
#include "../core/Protocol.h"
#include "../io/SchemaReaderIni.h"
#include "../io/SchemaReaderJson.h"
#include "../io/SchemaWriterJson.h"
#include "../io/CommonUtils.h"
#include "../widgets/ParamEditor.h"
#include "../windows/ExamplesDialog.h"
#include "../windows/PumpParamsDialog.h"
#include "../windows/SchemaPropsDialog.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "tools/OriMruList.h"
#include "tools/OriWaitCursor.h"
#include "widgets/OriSelectableTile.h"

#include <QApplication>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QTextStream>

using namespace Ori::Layouts;

static QFileDialog::Options fileDialogOptions()
{
    QFileDialog::Options options;
    if (!AppSettings::instance().useSystemDialogs)
        options |= QFileDialog::DontUseNativeDialog;
    return options;
}

static void startAppInstance(QStringList args, const QString& schemaFile = QString())
{
    QString exe = qApp->applicationFilePath();
    if (AppSettings::instance().isDevMode)
        args << "--dev";
    if (!schemaFile.isEmpty())
        args << schemaFile;
    if (!QProcess::startDetached(exe, args))
        qWarning() << "Unable to start another instance" << exe << args.join(' ');
}

//------------------------------------------------------------------------------

ProjectOperations::ProjectOperations(Schema *schema, QWidget *parent, CalcManager *calcManager, Ori::MruList* mruList) :
    QObject(parent), _parent(parent), _schema(schema), _calcManager(calcManager), _mruList(mruList)
{
    connect(_mruList, &Ori::MruList::clicked, this, [this](const QString& fileName){
        openSchemaFile(fileName, {.addToMru = true});
    });
}

QString ProjectOperations::getOpenFileName(QWidget* parent)
{
    QString recentPath = RecentData::getDir("schema_open_path");
    QString recentFilter = RecentData::getStr("schema_open_filter");

    auto fileName = QFileDialog::getOpenFileName(parent,
                                                 tr("Open schema", "Dialog title"),
                                                 recentPath,
                                                 Z::IO::Utils::filtersForOpen(),
                                                 &recentFilter,
                                                 fileDialogOptions());
    if (fileName.isEmpty()) return QString();

    RecentData::PendingSave _;
    RecentData::setDir("schema_open_path", fileName);
    RecentData::setStr("schema_open_filter", recentFilter);

    return Z::IO::Utils::refineFileName(fileName, recentFilter);
}

QString ProjectOperations::getSaveFileName(QWidget* parent)
{
    QString recentPath = RecentData::getDir("schema_save_path");
    QString recentFilter = RecentData::getStr("schema_save_filter");

    auto fileName = QFileDialog::getSaveFileName(parent,
                                                 tr("Save Schema", "Dialog title"),
                                                 recentPath,
                                                 Z::IO::Utils::filtersForSave(),
                                                 &recentFilter,
                                                 fileDialogOptions());
    if (fileName.isEmpty()) return QString();

    RecentData::PendingSave _;
    RecentData::setDir("schema_save_path", fileName);
    RecentData::setStr("schema_save_filter", recentFilter);

    return Z::IO::Utils::refineFileName(fileName, recentFilter);
}

void ProjectOperations::newSchemaFile()
{
    startAppInstance({});
}

void ProjectOperations::openExampleFile()
{
    auto fileName = selectSchemaExample();
    if (fileName.isEmpty()) return;
    openSchemaFile(fileName, {.isExample = true});
}

void ProjectOperations::openSchemaFile()
{
    auto fileName = getOpenFileName(_parent);
    if (fileName.isEmpty()) return;
    openSchemaFile(fileName, {.addToMru = true});
}

void ProjectOperations::openSchemaFile(const QString& fileName, const OpenFileOptions& opts)
{
    if (schema()->fileName() == fileName)
    {
        Z_REPORT("Already opened in the current window" << fileName)
        return;
    }

    if (!schema()->state().isNew())
    {
        QStringList args;
        if (opts.isExample)
            args << "--example";
        startAppInstance(args, fileName);
        return;
    }

    Z_REPORT("Loading" << fileName)
    Z::Report report;

    schema()->events().raise(SchemaEvents::Loading, "ProjectOperations: schema file opened");
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
        schema()->setTitle(fileInfo.completeBaseName());
        // Don't set file name as we don't want to overwrite examples,
        // and we don't want to show unexpected paths
        // somewhere in /tmp if the program is launched from AppImage
        // or somewhere inside of application bundle if we are on MacOS
    }
    else
        schema()->setFileName(fileName);
    schema()->events().enable();
    schema()->events().raise(SchemaEvents::Loaded, "ProjectOperations: schema file loaded");
    schema()->events().raise(SchemaEvents::RecalRequred, "ProjectOperations: schema file loaded");

    if (opts.addToMru)
        _mruList->append(fileName);
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
        QString msg = message % "\n\n" % tr("See the Protocol window for details.");
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
        return Ori::Dlg::yes(tr("The schema is in the old format and can not be saved, "
                                "would you like to save it as a file in the new format?"))
            && saveSchemaFileAs();
    return saveSchemaFile(fileName, {});
}

bool ProjectOperations::saveSchemaFileAs()
{
    auto fileName = getSaveFileName(_parent);
    if (fileName.isEmpty()) return false;
    return saveSchemaFile(fileName, {.addToMru = true});
}

void ProjectOperations::saveSchemaFileCopy()
{
    auto fileName = getSaveFileName(_parent);
    if (fileName.isEmpty()) return;
    saveSchemaFile(fileName, {.asCopy = true, .addToMru = true});
}

bool ProjectOperations::saveSchemaFile(const QString& fileName, const SaveFileOptions& opts)
{
    Z_REPORT("Saving" << fileName)

    if (schema()->memo && schema()->memo->editor)
    {
        auto editor = dynamic_cast<ISchemaMemoEditor*>(schema()->memo->editor.data());
        if (editor) editor->saveMemo();
    }

    SchemaWriterJson writer(schema());
    {
        Ori::WaitCursor wc;
        writer.writeToFile(fileName);
    }
    if (!writer.report().isEmpty())
        writeProtocol(writer.report(), tr("There are messages while saving project."));

    if (!writer.report().hasErrors() && !opts.asCopy)
    {
        schema()->setFileName(fileName);
        schema()->events().raise(SchemaEvents::Saved, "ProjectOperations: schema file saved");
    }

    bool ok = !writer.report().hasErrors();

    if (ok && opts.addToMru)
        _mruList->append(fileName);

    return ok;
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

Schema* ProjectOperations::createDefaultSchema(TripType tripType)
{
    auto schema = new Schema();
    schema->events().disable();
    schema->setTripType(tripType);
    if (tripType == TripType::SP)
        createDefaultPump(schema);
    schema->events().enable();
    return schema;
}

void ProjectOperations::createDefaultPump(Schema *schema)
{
    auto pump = PumpMode_Waist::instance()->makePump();
    pump->activate(true);

    if (AppSettings::instance().pumpAutoLabel)
        Z::Utils::generateLabel(schema, pump);

    schema->pumps()->append(pump);
    schema->events().raise(SchemaEvents::PumpCreated, pump, "ProjectOperations: default pump created");
}

bool ProjectOperations::editPumpDlg(PumpParams* pump)
{
    return PumpParamsDialog::editPump(pump, schema()->pumps());
}

void ProjectOperations::setupPump()
{
    if (!schema()->isSP()) return;

    if (schema()->pumps()->isEmpty())
        createDefaultPump(schema());

    auto pump = schema()->activePump();
    if (!pump) return;

    if (editPumpDlg(pump))
    {
        schema()->events().raise(SchemaEvents::PumpChanged, pump, "ProjectOperations: pump params changed via dialog");
        schema()->events().raise(SchemaEvents::RecalRequred, "ProjectOperations: pump params changed via dialog");
    }
}

void ProjectOperations::setupWavelength()
{
    ParamEditor editor(&schema()->wavelength());
    Ori::Dlg::Dialog(&editor, false)
            .withTitle(schema()->wavelength().name())
            .withIconPath(":/window_icons/wavelength")
            .withHorizontalPrompt(tr("Enter new wavelength:"))
            .withOnHelp([]{ Z::HelpSystem::topic("wavelen.html"); })
            .connectOkToContentApply()
            .exec();
}

bool ProjectOperations::selectTripTypeDlg(TripType* tripType)
{
    auto tripTypeLabel = new QLabel;
    auto font = tripTypeLabel->font();
    font.setPointSize(font.pointSize()+1);
    font.setBold(true);
    tripTypeLabel->setFont(font);

    Ori::Widgets::SelectableTileRadioGroup tripTypeGroup;
    connect(&tripTypeGroup, &Ori::Widgets::SelectableTileRadioGroup::dataSelected, this, [&](const QVariant& data){
        auto tripType = static_cast<TripType>(data.toInt());
        auto info = TripTypes::info(tripType);
        tripTypeLabel->setText(info.fullHeader());
    });

    Ori::Widgets::SelectableTile *activeTile = nullptr;

    auto tripTypeLayout = new QHBoxLayout();
    tripTypeLayout->setContentsMargins(0, 0, 0, 0);
    tripTypeLayout->setSpacing(12);
    for (auto tripType : TripTypes::all())
    {
        auto info = TripTypes::info(tripType);
        auto tile = new Ori::Widgets::SelectableTile;
        tile->setPixmap(QIcon(info.iconPath()).pixmap(32, 32));
        tile->setTitle(info.alias());
        tile->setToolTip(info.toolTip());
        tile->setData(static_cast<int>(tripType));
        tile->setTitleStyleSheet("font-weight:bold;font-size:15px;margin:10px 15px 0 15px;");

        if (tripType == schema()->tripType())
        {
            activeTile = tile;
            tile->setSelected(true);
            tripTypeLabel->setText(info.fullHeader());
        }

        tripTypeLayout->addWidget(tile);
        tripTypeGroup.addTile(tile);
    }

    QWidget content;
    LayoutV({tripTypeLabel, tripTypeLayout}).setMargin(0).setSpacing(12).useFor(&content);

    auto dlg = Ori::Dlg::Dialog(&content, false)
            .withTitle(tr("Round-trip type"))
            .withOnHelp([](){ Z::HelpSystem::instance()->showTopic("trip_type.html"); })
            .withContentToButtonsSpacingFactor(3)
            .withActiveWidget(activeTile)
            .withOkSignal(&tripTypeGroup, SIGNAL(doubleClicked(QVariant)));
    if (dlg.exec())
    {
        *tripType = static_cast<TripType>(tripTypeGroup.selectedData().toInt());
        return true;
    }
    return false;
}

void ProjectOperations::setupTripType()
{
    TripType tripType;
    if (selectTripTypeDlg(&tripType))
    {
        if (tripType == TripType::SP && schema()->pumps()->isEmpty())
            createDefaultPump(schema());
        schema()->setTripType(tripType);
    }
}

QString ProjectOperations::selectSchemaExample()
{
    return ExamplesDialog::exec();
}

void ProjectOperations::editSchemaProps()
{
   Z::Dlg::editSchemaProps(_parent, schema());
}
