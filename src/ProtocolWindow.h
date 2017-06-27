#ifndef PROTOCOL_WINDOW_H
#define PROTOCOL_WINDOW_H

#include "SchemaWindows.h"

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
QT_END_NAMESPACE

class ProtocolWindow : public BasicMdiChild
{
    Q_OBJECT

public:
    ~ProtocolWindow();

    static ProtocolWindow* create();

private:
    explicit ProtocolWindow();

    static ProtocolWindow* _instance;

    QPlainTextEdit* _log;
};

#endif // PROTOCOL_WINDOW_H
