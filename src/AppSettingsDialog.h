#ifndef CONFIG_DLG
#define CONFIG_DLG

#include "dialogs/OriBasicConfigDlg.h"

QT_BEGIN_NAMESPACE
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
    AppSettingsDialog(QWidget* parent);

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

    QWidget* createGeneralPage();
    QWidget* createViewPage();
    QWidget* createLayoutPage();
    QWidget* createUnitsPage();
    QWidget* createExportPage();
    QWidget* createCalcPage();
    void fillLangsCombo();
    void fillStylesCombo();
};

#ifndef DLG_APP_CONFIG
#define DLG_APP_CONFIG
namespace Z {
namespace Dlg {

bool editAppSettings(QWidget* parent);

} // namespace Dlg
} // namespace Z
#endif

#endif // CONFIG_DLG
