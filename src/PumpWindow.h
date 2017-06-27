#ifndef PUMPWINDOW_H
#define PUMPWINDOW_H

#include "RezonatorDialog.h"
#include "core/Types.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class PumpWindow : public RezonatorDialog
{
    Q_OBJECT
public:
    explicit PumpWindow(QWidget *parent, class Schema* schema);

    static QByteArray savedGeometry;

private:
    class ValuesEditorTS *_paramEditors;
    class Schema *_schema;
    Z::PumpParams _params;
    QLabel* _drawing;

private slots:
    void inputTypeChanged(int mode);
    void collect() override;
};


namespace Z {
namespace Dlgs {

#ifndef DLG_PUMP
#define DLG_PUMP
bool pumpParams(QWidget *parent, Schema *schema);
#endif

} // namespace Dlgs
} // namespace Z

#endif // PUMPWINDOW_H
