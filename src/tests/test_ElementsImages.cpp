#include "testing/OriTestBase.h"
#include "../core/ElementsCatalog.h"
#include "../widgets/ElementImagesProvider.h"
#include "../widgets/SchemaLayout.h"

#include <QPixmap>
#include <QSvgRenderer>

namespace Z {
namespace Tests {
namespace ElementsImagesTests {

TEST_METHOD(resource_icons)
{
    bool ok = true;
    for (auto elem : ElementsCatalog::instance().elements())
    {
        TEST_LOG(elem->type())
        auto path = ElementImagesProvider::instance().iconPath(elem->type());
        QPixmap icon(path);
        if (icon.isNull())
        {
            TEST_LOG("    no icon: " + path)
            ok = false;
        }
    }
    if (!ok) ASSERT_FAIL("Not all elements have icon")
}

TEST_METHOD(resource_outlines)
{
    bool ok = true;
    QSvgRenderer outline;
    for (auto elem : ElementsCatalog::instance().elements())
    {
        TEST_LOG(elem->type())
        auto path = ElementImagesProvider::instance().drawingPath(elem->type());
        if (!outline.load(path))
        {
            TEST_LOG("    no outline: " + path)
            ok = false;
        }
    }
    if (!ok) ASSERT_FAIL("Not all elements have outline")
}

TEST_METHOD(layouts)
{
    bool ok = true;
    for (auto elem : ElementsCatalog::instance().elements())
    {
        TEST_LOG(elem->type())
        QSharedPointer<ElementLayout> layout(ElementLayoutFactory::make(elem));
        if (!layout)
        {
            TEST_LOG("    no layout")
            ok = false;
        }
    }
    if (!ok) ASSERT_FAIL("Not all elements have layout")
}

//------------------------------------------------------------------------------

TEST_GROUP("Elements Images",
           ADD_TEST(resource_icons),
           ADD_TEST(resource_outlines),
           ADD_TEST(layouts),
           )

} // namespace ElementsImagesTests
} // namespace Tests
} // namespace Z
