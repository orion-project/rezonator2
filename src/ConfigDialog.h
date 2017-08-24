#ifndef CONFIG_DLG
#define CONFIG_DLG

#include "dialogs/OriBasicConfigDlg.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QSpinBox;
class OptionsGroup;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {
    class OptionsGroup;
}}

class ConfigDialog : public Ori::Dlg::BasicConfigDialog
{
    Q_OBJECT

public:
    ConfigDialog(QWidget* parent);
    ~ConfigDialog();

    ///// inherited from BasicConfigDialog
    virtual void populate() override;
    virtual bool collect() override;

private:
    Ori::Widgets::OptionsGroup *groupOptions;
    Ori::Widgets::OptionsGroup *groupView;
    QSpinBox *sedMruLength;

    QWidget* createGeneralPage();
    QWidget* createViewPage();
    QWidget* createLayoutPage();
    void fillLangsCombo();
    void fillStylesCombo();

    static int _savedTabIndex;
};

#ifndef DLG_APP_CONFIG
#define DLG_APP_CONFIG
namespace Z {
namespace Dlg {

bool appConfig(QWidget* parent);

} // namespace Dlg
} // namespace Z
#endif

#endif // CONFIG_DLG
