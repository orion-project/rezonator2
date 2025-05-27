#include "../widgets/ParamEditor.h"

#include "testing/OriTestBase.h"

namespace Z {
namespace Tests {
namespace ParamEditorTests {

//------------------------------------------------------------------------------

namespace {

#define PARAM_DESTRUCTED QString("param was deleted")

class TestParam : public Z::Parameter
{
public:
    TestParam(Ori::Testing::TestBase *test) : _test(test) {}
    ~TestParam() override {
        _test->data().insert(PARAM_DESTRUCTED, PARAM_DESTRUCTED);
    }
    QVector<ParameterListener*> listeners() const { return _listeners; }
public:
    Ori::Testing::TestBase *_test;
};
} // namespace

TEST_METHOD(ctor__must_register_listener)
{
    TestParam param(test);
    QSharedPointer<ParamEditor> editor(new ParamEditor(ParamEditor::Options(&param)));
    ASSERT_IS_TRUE(param.listeners().contains(editor.data()))
}

TEST_METHOD(destructor__must_unregister_listener_when_no_own)
{
    TestParam param(test);
    ParamEditor::Options opts(&param);
    auto editor = new ParamEditor(opts);
    ASSERT_IS_TRUE(param.listeners().contains(editor))
    delete editor;
    ASSERT_IS_FALSE(param.listeners().contains(editor))
    ASSERT_IS_FALSE(test->data().contains(PARAM_DESTRUCTED))
}

TEST_METHOD(destructor__must_delete_param_when_own)
{
    auto param = new TestParam(test);
    ParamEditor::Options opts(param);
    opts.ownParam = true;
    auto editor = new ParamEditor(opts);
    delete editor;
    ASSERT_IS_TRUE(test->data().contains(PARAM_DESTRUCTED))
}

//------------------------------------------------------------------------------

TEST_GROUP("ParamEditor",
    ADD_GUI_TEST(ctor__must_register_listener),
    ADD_GUI_TEST(destructor__must_unregister_listener_when_no_own),
    ADD_GUI_TEST(destructor__must_delete_param_when_own),
)

} // namespace ParamEditorTests
} // namespace Tests
} // namespace Z
