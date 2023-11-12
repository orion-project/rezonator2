#include "../widgets/ParamsEditor.h"

#include "testing/OriTestBase.h"

namespace Z {
namespace Tests {
namespace ParamsEditorTests {

//------------------------------------------------------------------------------

namespace {
const QString PARAM_DESTRUCTED("param was deleted ");
class TestParam : public Z::Parameter
{
public:
    TestParam(Ori::Testing::TestBase *test, const QString& name) : paramName(name), _test(test) {}
    ~TestParam() override {
        _test->data().insert(PARAM_DESTRUCTED + paramName, true);
    }
    QVector<ParameterListener*> listeners() const { return _listeners; }
    QString paramName;
public:
    Ori::Testing::TestBase *_test;
};
} // namespace

TEST_METHOD(ctor__must_register_listeners)
{
    TestParam p1(test, "p1");
    TestParam p2(test, "p2");
    Z::Parameters params = {&p1, &p2};
    ParamsEditor::Options opts(&params);
    QSharedPointer<ParamsEditor> editor(new ParamsEditor(opts));
    ASSERT_IS_FALSE(p1.listeners().isEmpty())
    ASSERT_IS_FALSE(p2.listeners().isEmpty())
}

TEST_METHOD(destructor__must_unregister_listeners_when_no_own)
{
    TestParam p1(test, "p1");
    TestParam p2(test, "p2");
    Z::Parameters params = {&p1, &p2};
    ParamsEditor::Options opts(&params);
    auto editor = new ParamsEditor(opts);
    ASSERT_IS_FALSE(p1.listeners().isEmpty())
    ASSERT_IS_FALSE(p2.listeners().isEmpty())
    delete editor;
    ASSERT_IS_TRUE(p1.listeners().isEmpty())
    ASSERT_IS_TRUE(p2.listeners().isEmpty())
    ASSERT_IS_FALSE(test->data().contains(PARAM_DESTRUCTED + p1.paramName))
    ASSERT_IS_FALSE(test->data().contains(PARAM_DESTRUCTED + p2.paramName))
}

TEST_METHOD(destructor__must_delete_params_when_own)
{
    auto p1 = new TestParam(test, "p1");
    auto p2 = new TestParam(test, "p2");
    Z::Parameters params = {p1, p2};
    ParamsEditor::Options opts(&params);
    opts.ownParams = true;
    auto editor = new ParamsEditor(opts);
    delete editor;
    ASSERT_IS_TRUE(test->data().contains(PARAM_DESTRUCTED + "p1"))
    ASSERT_IS_TRUE(test->data().contains(PARAM_DESTRUCTED + "p2"))
}

//------------------------------------------------------------------------------

TEST_GROUP("ParamsEditor",
    ADD_TEST(ctor__must_register_listeners),
    ADD_TEST(destructor__must_unregister_listeners_when_no_own),
    ADD_TEST(destructor__must_delete_params_when_own),
)

} // namespace ParamEditorTests
} // namespace Tests
} // namespace Z
