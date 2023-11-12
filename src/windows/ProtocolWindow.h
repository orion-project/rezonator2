#ifndef PROTOCOL_WINDOW_H
#define PROTOCOL_WINDOW_H

#include "SchemaWindows.h"

QT_BEGIN_NAMESPACE
class QMenu;
class QPlainTextEdit;
QT_END_NAMESPACE

class ProtocolWindow : public BasicMdiChild
{
    Q_OBJECT

public:
    ~ProtocolWindow() override;

    static ProtocolWindow* create();

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return { _windowMenu }; }

private:
    explicit ProtocolWindow();

    static ProtocolWindow* _instance;

    QPlainTextEdit* _log;
    QMenu* _windowMenu;
};

#endif // PROTOCOL_WINDOW_H
