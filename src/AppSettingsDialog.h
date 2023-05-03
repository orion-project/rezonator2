#ifndef CONFIG_DLG
#define CONFIG_DLG

#include "dialogs/OriBasicConfigDlg.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QSpinBox;
QT_END_NAMESPACE

class UnitComboBox;

namespace Ori {
namespace Widgets {
    class OptionsGroupV2;
}}

class AppSettingsDialog : public Ori::Dlg::BasicConfigDialog
{
    Q_OBJECT

public:
    AppSettingsDialog(QWidget* parent, Ori::Dlg::PageId currentPageId);

    // inherited from BasicConfigDialog
    virtual void populate() override;
    virtual bool collect() override;

private:
    Ori::Widgets::OptionsGroupV2 *_groupOptions;
    Ori::Widgets::OptionsGroupV2 *_groupView;
    Ori::Widgets::OptionsGroupV2 *_groupLayoutExport;
    Ori::Widgets::OptionsGroupV2 *_groupExportData;
    Ori::Widgets::OptionsGroupV2 *_groupExportPlot;
    Ori::Widgets::OptionsGroupV2 *_groupCalcOpts;
    UnitComboBox *_defaultUnitBeamRadius;
    UnitComboBox *_defaultUnitFrontRadius;
    UnitComboBox *_defaultUnitAngle;
    QSpinBox *_exportNumberPrecision;
    QSpinBox *_numberPrecisionData;
    QCheckBox *_showImagUnitAsJ, *_showImagUnitAtEnd;

    QWidget* createGeneralPage();
    QWidget* createViewPage();
    QWidget* createLayoutPage();
    QWidget* createUnitsPage();
    QWidget* createExportPage();
    QWidget* createCalcPage();
    void fillLangsCombo();
    void fillStylesCombo();
};

#endif // CONFIG_DLG
