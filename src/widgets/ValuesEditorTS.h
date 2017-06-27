#ifndef VALUESEDITORTS_H
#define VALUESEDITORTS_H

#include <QMap>
#include <QWidget>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
class QLabel;
class QToolButton;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {
    class ValueEdit;
}}
namespace Z {
    struct ValueTS;
}

class ValueEditorTS : public QWidget
{
    Q_OBJECT
public:
    explicit ValueEditorTS(QWidget *parent = 0);
    void collect();
    double valueT() const;
    double valueS() const;
    void setValueT(const double& value);
    void setValueS(const double& value);
    void setValue(Z::ValueTS *value);
    void setLabel(const QString& label);
    void setSymbol(const QString& symbol);
    QLabel* symbolLabel() const { return _symbol; }
public slots:
    void swapValues();
    void assignTtoS();
    void assignStoT();
    void linkValues();
signals:
    void goingFocusNext();
    void goingFocusPrev();
private:
    Z::ValueTS *_value = nullptr;
    Ori::Widgets::ValueEdit *_editorT, *_editorS;
    QLabel *_label, *_symbol;
private slots:
    void editorFocused(bool focus);
    void editorKeyPressed(int key);
};

class ValuesEditorTS : public QVBoxLayout
{
    Q_OBJECT
public:
    explicit ValuesEditorTS(QWidget *parent = 0);
    void addEditor(int id, Z::ValueTS *value);
    double valueT(int id) const;
    double valueS(int id) const;
    void setValueT(int id, const double& value);
    void setValueS(int id, const double& value);
    void setValue(int id, Z::ValueTS *value);
    void setLabel(int id, const QString& label);
    void setSymbol(int id, const QString& symbol);
    void setVisible(int id, bool visible);
    void collect();
    //void adjustSymbolsWidth();
public slots:
    void swapValues();
    void assignTtoS();
    void assignStoT();
    void linkValues();
private:
    QMap<int, ValueEditorTS*> _editors;
    ValueEditorTS* getEditor(int id) const;
};

#endif // VALUESEDITORTS_H
