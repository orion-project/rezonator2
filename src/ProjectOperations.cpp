#include "AppSettings.h"
#include "CalcManager.h"
#include "ProjectOperations.h"
#include "io/z_io_utils.h"
#include "io/SchemaReaderIni.h"
#include "io/SchemaReaderXml.h"
#include "io/SchemaWriterXml.h"
#include "core/Schema.h"
#include "core/Protocol.h"
#include "widgets/ParamEditor.h"
#include "helpers/OriDialogs.h"
#include "tools/OriWaitCursor.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QTextStream>

QFileDialog::Options fileDialogOptions()
{
    QFileDialog::Options options = 0;
    if (!Settings::instance().useSystemDialogs)
        options |= QFileDialog::DontUseNativeDialog;
    return options;
}

////////////////////////////////////////////////////////////////////////////////

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
    Settings& settings = Settings::instance();
    auto fileName = QFileDialog::getOpenFileName(_parent,
                                                 tr("Open schema", "Dialog title"),
                                                 settings.schemaOpenPath(),
                                                 Z::IO::Utils::filtersForOpen(),
                                                 settings.schemaOpenFilter(),
                                                 fileDialogOptions());
    if (fileName.isEmpty()) return QString();

    settings.schemaFileDlgOpenPath = QFileInfo(fileName).absolutePath();

    fileName = Z::IO::Utils::refineFileName(fileName, settings.schemaFileDlgOpenFilter);

    emit fileNameSelected(fileName);
    return fileName;
}

QString ProjectOperations::getSaveFileName()
{
    Settings& settings = Settings::instance();
    auto fileName = QFileDialog::getSaveFileName(_parent,
                                                 tr("Save Schema", "Dialog title"),
                                                 settings.schemaSavePath(),
                                                 Z::IO::Utils::filtersForSave(),
                                                 settings.schemaSaveFilter(),
                                                 fileDialogOptions());
    if (fileName.isEmpty()) return QString();

    settings.schemaFileDlgSavePath = QFileInfo(fileName).absolutePath();

    fileName = Z::IO::Utils::refineFileName(fileName, settings.schemaFileDlgSaveFilter);

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
    Z_REPORT("Loading" << fileName)

    if (!schema()->state().isNew())
    {
        QProcess::startDetached(qApp->applicationFilePath() + " \"" + fileName + "\"");
        return;
    }

    Ori::WaitCursor wc;

    SchemaFile* reader = Z::IO::Utils::isOldSchema(fileName)
        ? (SchemaFile*) new SchemaReaderIni(schema(), fileName)
        : (SchemaFile*) new SchemaReaderXml(schema(), fileName);

    reader->read();

    if (!reader->report().IsEmpty())
        writeProtocol(reader->report(), tr("There are messages while loading project."));

    delete reader;
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

    Ori::WaitCursor wc;

    SchemaWriterXml writer(schema(), fileName);
    writer.write();

    if (!writer.report().IsEmpty())
        writeProtocol(writer.report(), tr("There are messages while saving project."));

    return writer.ok();
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

    SchemaWriterXml writer(schema());
    QString text = writer.writeToString();

    if (!writer.report().IsEmpty())
        writeProtocol(writer.report(), tr("There are messages while saving copy of project."));

    if (writer.ok())
    {
        auto res = saveText(text, fileName);
        if (!res.isEmpty())
            Ori::Dlg::error(tr("Error while saving copy of project:\n\n%1").arg(res));
    }
}

QString ProjectOperations::saveText(const QString& text, const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return file.errorString();
    QTextStream stream(&file);
    stream << text;
    file.close();
    return QString();
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

void ProjectOperations::setupSchema()
{
    //Z::Dlgs::schemaParams(_parent, schema());
}

void ProjectOperations::setupPump()
{
//    if (schema()->tripType() == Schema::SP)
//        if (Z::Dlgs::pumpParams(_parent, schema()))
//        {
//            // TODO
//        }
}

void ProjectOperations::setupWavelength()
{
    ParamEditor editor(&schema()->wavelength(), false);
    // TODO extend showDialogWithPromptH method to accept help topic
    Ori::Dlg::showDialogWithPromptH(tr("Enter new wavelength:"), &editor,
                                    schema()->wavelength().name(), ":/window_icons/wavelength");
}

