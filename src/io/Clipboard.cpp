#include "Clipboard.h"

#include "../core/Report.h"
#include "SchemaWriterJson.h"
#include "SchemaReaderJson.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeData>

namespace Z {
namespace IO {
namespace Clipboard {

const QString Z_CLIPBOARD_MIME_TYPE = "application/x-rezonator2-mime";
const QString Z_CLIPBOARD_DATA_ELEMENTS = Z_CLIPBOARD_MIME_TYPE + ";value=elements";

void setClipboardData(QJsonObject& root, const QString& dataType)
{
    root["data_type"] = dataType;

    QString text = QJsonDocument(root).toJson();

    auto mimeData = new QMimeData;
    mimeData->setData(Z_CLIPBOARD_MIME_TYPE, text.toUtf8());
    qApp->clipboard()->setMimeData(mimeData);
}

QJsonObject getClipboradData(const QString& expectedType)
{
    auto mimeData = qApp->clipboard()->mimeData();
    if (!mimeData)
    {
        qDebug() << "Clipboard doesn't contain a data";
        return {};
    }

    auto data = mimeData->data(Z_CLIPBOARD_MIME_TYPE);
    if (data.isNull())
    {
        qDebug() << "Clipboard doesn't contain a data of suitable mime-type" << Z_CLIPBOARD_MIME_TYPE;
        return {};
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (doc.isNull())
    {
        qDebug() << "Clipboard text is not a valid JSON object" << error.errorString();
        return {};
    }

    QJsonObject root = doc.object();
    QString dataType = root["data_type"].toString();
    if (dataType != expectedType)
    {
        qDebug() << "There is no data of appropriate type in Clipboard"
                 << "Expected type:" << expectedType
                 << "Actual type:" << dataType;
        return {};
    }

    return root;
}

void setElements(const QList<Element*>& elements)
{
    QJsonObject root;
    Z::IO::Json::writeElements(root, elements);
    setClipboardData(root, Z_CLIPBOARD_DATA_ELEMENTS);
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
