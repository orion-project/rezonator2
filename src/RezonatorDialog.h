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
        NoOptions = 0x00,
        OmitButtonsPanel = 0x01,
        UseHelpButton = 0x02,
        DontDeleteOnClose = 0x04,
    };
    Q_DECLARE_FLAGS(Options, Option)

public:
    explicit RezonatorDialog(Options options = NoOptions, QWidget *parent = 0);
    ~RezonatorDialog();

    void setTitleAndIcon(const QString& title, const char* iconPath);

    QVBoxLayout* mainLayout() const { return _mainLayout; }

protected slots:
    virtual void collect() { accept(); }

protected:
    void showEvent(QShowEvent *event);

    virtual QString helpTopic() const { return ""; }

private:
    QVBoxLayout* _mainLayout;

    void showHelp();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(RezonatorDialog::Options)

#endif // REZONATOR_DIALOG_H
