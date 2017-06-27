#ifndef SCHEMA_CLIENT_H
#define SCHEMA_CLIENT_H

#include <QList>
#include <QVector>

class SchemaClient
{
public:
    virtual ~SchemaClient() {}
};


class SchemaClients
{
public:
    void append(SchemaClient *client)
    {
        if (!_items.contains(client))
            _items.append(client);
    }

    void remove(SchemaClient *client)
    {
        _items.removeOne(client);
    }

    const QList<SchemaClient*>& items() const { return _items; }

    template <class T> QVector<T*> get() const;

protected:
    QList<SchemaClient*> _items;
};



template <class T> QVector<T*> SchemaClients::get() const
{
    QVector<T*> items;
    for (SchemaClient* item : _items)
    {
        T* t = dynamic_cast<T*>(item);
        if (t) items.append(t);
    }
    return items;
}

#endif // SCHEMA_CLIENT_H
