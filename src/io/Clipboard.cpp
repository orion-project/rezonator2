#include "Clipboard.h"

namespace Z {
namespace IO {
namespace Clipboard {

void copyElements(const QList<Element*>& elements)
{
    // TODO:NEXT-VER
    //    Elements elems = _table->selection();
    //    if (!elems.empty())
    //    {
    //        Schema *tmp_schema = new Schema;
    //        for (int i = 0; i < elems.size(); i++)
    //        {
    //            Element *elem = ElementsCatalog::instance().create(elems.at(i)->type());
    //            elem->params().setValues(elems.at(i)->params().getValues());
    //            tmp_schema->insertElement(elem, -1, false);
    //        }
    //        QString text;
    //        SchemaWriterXml file(tmp_schema);
    //        file.write(&text);
    //        QApplication::clipboard()->setText(text);
    //        delete tmp_schema;
    //    }

}

void pasteElements(Schema* schema)
{
    // TODO:NEXT-VER
    //    QString text = QApplication::clipboard()->text();
    //    if (!text.isEmpty())
    //    {
    //        Schema tmp_schema;
    //        SchemaReaderXml file(&tmp_schema);
    //        file.read(text);
    //        if (file.ok() && tmp_schema.count() > 0)
    //        {
    //            int count = tmp_schema.count();
    //            Element* elems[count];
    //            for (int i = 0; i < count; i++)
    //                elems[i] = tmp_schema.element(i);
    //            _pasteMode = true;
    //            for (int i = 0; i < count; i++)
    //            {
    //                tmp_schema.deleteElement(elems[i], false);
    //                schema()->insertElement(elems[i], -1, true);
    //            }
    //            _pasteMode = false;
    //        }
    //    }
}

} // namespace Clipboard
} // namespace IO
} // namespace Z
