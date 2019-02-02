#include "Clipboard.h"

#include "../core/Report.h"
#include "SchemaWriterJson.h"
#include "SchemaReaderJson.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

namespace Z {
namespace IO {
namespace Clipboard {

const QString Z_CLIPBOARD_DATA_ELEMENTS = "application/x-rezonator2-mime;value=elements";

void setElements(const QList<Element*>& elements)
{
    QJsonObject root;
    root["data_type"] = Z_CLIPBOARD_DATA_ELEMENTS;

    Z::IO::Json::writeElements(root, elements);

    qApp->clipboard()->setText(QJsonDocument(root).toJson());
}

QJsonObject getClipboradData(const QString& expectedType)
{
    QString text = qApp->clipboard()->text();
    if (text.isEmpty()) {
        qDebug() << "Clipboard doesn't contain a text data";
        return QJsonObject();
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &error);
    if (doc.isNull())
    {
        qDebug() << "Clipboard text is not a valid JSON object" << error.errorString();
        return QJsonObject();
    }

    QJsonObject root = doc.object();
    QString dataType = root["data_type"].toString();
    if (dataType != expectedType)
    {
        qDebug() << "There is no data of appropriate type in Clipboard"
                 << "Expected type:" << expectedType
                 << "Actual type:" << dataType;
        return QJsonObject();
    }

    return root;
}

QList<Element*> getElements()
{
    QJsonObject root = getClipboradData(Z_CLIPBOARD_DATA_ELEMENTS);
    if (root.isEmpty()) return {};

    Z::Report report;
    auto elems = Z::IO::Json::readElements(root, &report);
    if (!report.isEmpty())
        qDebug() << report.str();

    return elems;
}

} // namespace Clipboard
} // namespace IO
} // namespace Z
