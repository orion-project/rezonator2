#include "CommonData.h"
#include "CustomPrefs.h"
#include "tools/OriSettings.h"
#include "tools/OriStyler.h"
#include "tools/OriTranslator.h"
#include "tools/OriMruList.h"

#include <QDebug>

CommonData* CommonData::_instance = nullptr;

CommonData::CommonData(QObject *parent) : QObject(parent)
{
    if (_instance)
    {
        qWarning() << "Only first instance of CommonData object is useful";
        return;
    }

    _instance = this;
    Settings::instance().registerListener(this);

    Ori::Settings s;
    s.beginGroup("View");
    _styler = new Ori::Styler(s.strValue("style"), this);
    _translator = new Ori::Translator(s.strValue("language"), this);

    s.beginGroup("States");
    _mruList = new Ori::MruFileList(this);
    _mruList->setMaxCount(Settings::instance().mruSchemaCount);
    _mruList->load(s.settings());

    CustomPrefs::load(s.settings()->fileName());
}

CommonData::~CommonData()
{
    Ori::Settings s;
    s.beginGroup("View");
    s.setValue("style", _styler->currentStyle());
    s.setValue("language", _translator->currentLanguage());

    _instance = nullptr;
    Settings::instance().unregisterListener(this);
}

void CommonData::settingsChanged()
{
    _mruList->setMaxCount(Settings::instance().mruSchemaCount);
}

void CommonData::addFileToMruList(const QString& fileName)
{
    _mruList->append(fileName);
}

