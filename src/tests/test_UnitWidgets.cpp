#include "testing/OriTestBase.h"
#include "../widgets/UnitWidgets.h"
#include "TestUtils.h"

#include <QMenu>

namespace Z {
namespace Tests {
namespace UnitWidgetsTests {

template <typename T> QSet<T> listToSet(const QList<T>& list)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    return QSet<T>(list.begin(), list.end());
#else
    return list.toSet();
#endif
}

//------------------------------------------------------------------------------

namespace UnitComboBoxTests {

Z::Unit getUnit(const UnitComboBox& combo, int index)
{
    return combo.itemData(index).value<Z::Unit>();
}

TEST_METHOD(must_contain_all_units)
{
    UnitComboBox combo;
    for (auto dim : Z::Dims::dims())
    {
        combo.populate(dim);

        for (auto unit : dim->units())
        {
            combo.setSelectedUnit(unit);
            ASSERT_IS_TRUE(combo.selectedUnit() == unit);
        }

        auto units = listToSet(dim->units());
        for (int i = 0; i < combo.count(); i++)
            units.remove(getUnit(combo, i));
        ASSERT_IS_TRUE(units.isEmpty());
    }
}

TEST_METHOD(must_disable_unit_none)
{
    UnitComboBox combo(Z::Dims::none());
    ASSERT_EQ_INT(combo.count(), 1)
    ASSERT_IS_TRUE(combo.selectedUnit() == Z::Units::none())
    ASSERT_IS_FALSE(combo.isEnabled())
}

TEST_METHOD(must_disable_unit_fixed)
{
    UnitComboBox combo(Z::Dims::fixed());
    ASSERT_IS_FALSE(combo.isEnabled())
    combo.setEnabled(true);
    ASSERT_IS_FALSE(combo.isEnabled())
}

TEST_METHOD(must_enable_unit_fixed_when_required)
{
    UnitComboBox combo;
    combo.canSelectFixedUnit = true;
    combo.populate(Z::Dims::fixed());
    ASSERT_IS_TRUE(combo.isEnabled())
    combo.setEnabled(false);
    ASSERT_IS_FALSE(combo.isEnabled())
}

TEST_GROUP("UnitComboBox",
    ADD_TEST(must_contain_all_units),
    ADD_TEST(must_disable_unit_none),
    ADD_TEST(must_disable_unit_fixed),
    ADD_TEST(must_enable_unit_fixed_when_required),
)

} // namespace UnitComboBoxTests

//------------------------------------------------------------------------------

namespace DimComboBoxTests {

TEST_METHOD(must_contain_all_dims)
{
    DimComboBox combo;
    auto dims = listToSet(Z::Dims::dims());
    for (int i = 0; i < combo.count(); i++)
        dims.remove(combo.itemData(i).value<Z::Dim>());
    ASSERT_IS_TRUE(dims.isEmpty())
}

TEST_METHOD(must_select_dim)
{
    DimComboBox combo;
    for (auto dim : Z::Dims::dims())
    {
        combo.setSelectedDim(dim);
        ASSERT_IS_TRUE(dim == combo.selectedDim())
    }
}

TEST_GROUP("DimComboBox",
    ADD_TEST(must_contain_all_dims),
    ADD_TEST(must_select_dim),
)

} // namespace DimComboBoxTests

//------------------------------------------------------------------------------

namespace UnitsMenuTests {

TEST_CASE_METHOD(must_contain_all_units, Z::Dim dim)
{
    UnitsMenu m;
    for (Z::Unit selectedUnit : dim->units())
    {
        m.setUnit(selectedUnit);

        auto units = listToSet(dim->units());
        for (auto action : m.menu()->actions())
        {
            auto unit = action->data().value<Z::Unit>();
            units.remove(unit);

            if (unit == selectedUnit)
                ASSERT_IS_TRUE(action->isChecked())
            else
                ASSERT_IS_FALSE(action->isChecked())
        }
        ASSERT_IS_TRUE(units.isEmpty());
    }
}

TEST_CASE_METHOD(must_contain_no_units, Z::Dim dim)
{
    UnitsMenu m;
    for (Z::Unit selectedUnit : dim->units())
    {
        m.setUnit(selectedUnit);
        ASSERT_IS_TRUE(m.menu()->isEmpty());
        ASSERT_IS_FALSE(m.menu()->isEnabled());
    }
}

TEST_CASE(must_contain_all_units_linear, must_contain_all_units, Z::Dims::linear())
TEST_CASE(must_contain_all_units_angular, must_contain_all_units, Z::Dims::angular())
TEST_CASE(must_contain_no_units_none, must_contain_no_units, Z::Dims::none())
TEST_CASE(must_contain_no_units_fixed, must_contain_no_units, Z::Dims::fixed())

TEST_GROUP("UnitsMenu",
    ADD_TEST(must_contain_all_units_linear),
    ADD_TEST(must_contain_all_units_angular),
    ADD_TEST(must_contain_no_units_none),
    ADD_TEST(must_contain_no_units_fixed),
)

} // namespace UnitsMenu

//------------------------------------------------------------------------------

TEST_GROUP("Unit Widgets",
    ADD_GROUP(UnitComboBoxTests),
    ADD_GROUP(DimComboBoxTests),
    ADD_GROUP(UnitsMenuTests),
)

} // namespace UnitWidgetsTests
} // namespace Tests
} // namespace Z
