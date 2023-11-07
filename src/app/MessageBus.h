#ifndef MESSAGE_BUS_H
#define MESSAGE_BUS_H

#include "../core/Units.h"

enum MessageBusEvent {
    MBE_MEMO_ADDED,
    MBE_MEMO_REMOVED,
};

class IMessageBusListener
{
public:
    virtual void messageBusEvent(MessageBusEvent event, const QMap<QString, QVariant>& params) = 0;
};

class MessageBus :
        public Ori::Singleton<MessageBus>,
        public Ori::Notifier<IMessageBusListener>
{
public:
    void send(MessageBusEvent event, const QMap<QString, QVariant>& params);

private:
    MessageBus() {}

    friend class Ori::Singleton<MessageBus>;
};

#endif // MESSAGE_BUS_H
