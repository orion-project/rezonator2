#ifndef REZONATOR_DIALOG_H
#define REZONATOR_DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

class RezonatorDialog : public QDialog
{
    Q_OBJECT

public:
    enum Option {
        NoOptions = 0x00,         ///< Default dialog with OK and Cancel, auto-deletable.
        OmitButtonsPanel = 0x01,  ///< Do not create dialog buttons panel.
        UseHelpButton = 0x02,     ///< Add Help button on dialog panel,
                                  ///< helpTopic() method should return correct value.
        DontDeleteOnClose = 0x04, ///< Do not auto-delete dialog after closing,
                                  ///< caller is responsible for deletion.
    };
    Q_DECLARE_FLAGS(Options, Option)

public:
    explicit RezonatorDialog(Options options = NoOptions, QWidget *parent = nullptr);
    ~RezonatorDialog();

    void setTitleAndIcon(const QString& title, const char* iconPath);

    /// The layout that should be used to place content in.
    QVBoxLayout* mainLayout() const { return _mainLayout; }

    bool run();

protected slots:
    virtual void collect() { accept(); }

protected:
    void showEvent(QShowEvent *event);

    virtual QString helpTopic() const { return ""; }

    /// Returns size of dialog that should be used when no stored geometry is found.
    virtual QSize prefferedSize() const { return QSize(0, 0); }

private:
    QVBoxLayout* _mainLayout;

    void showHelp();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(RezonatorDialog::Options)

#endif // REZONATOR_DIALOG_H
