#include "MessageBus.h"

#include <QMap>
#include <QVariant>

void MessageBus::send(MessageBusEvent event, const QMap<QString, QVariant>& params)
{
    notify(&IMessageBusListener::messageBusEvent, event, params);
}
