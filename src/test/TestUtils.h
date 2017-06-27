#ifndef Z_TEST_UTILS_H
#define Z_TEST_UTILS_H

inline double frand(double min, double max)
{
    return min + (double(qrand()) / double(RAND_MAX)) * (max - min);
}

#define RAD_TO_DEG(a) ((a)*180.0/M_PI)
#define DEG_TO_RAD(a) ((a)*M_PI/180.0)

#define ASSERT_MATRIX_IS(m, a, b, c, d) \
    ASSERT_EQ_DBL((m).A, a); \
    ASSERT_EQ_DBL((m).B, b); \
    ASSERT_EQ_DBL((m).C, c); \
    ASSERT_EQ_DBL((m).D, d);

#define ASSERT_MATRIX_IS_NOT(m, a, b, c, d) \
    ASSERT_NEQ_DBL((m).A, a); \
    ASSERT_NEQ_DBL((m).B, b); \
    ASSERT_NEQ_DBL((m).C, c); \
    ASSERT_NEQ_DBL((m).D, d);

#define ASSERT_EQ_MATRIX(m, expected) \
    ASSERT_EQ_DBL((m).A, (expected).A); \
    ASSERT_EQ_DBL((m).B, (expected).B); \
    ASSERT_EQ_DBL((m).C, (expected).C); \
    ASSERT_EQ_DBL((m).D, (expected).D);

#define ASSERT_MATRIX_IS_UNITY(m) ASSERT_MATRIX_IS(m, 1.0, 0.0, 0.0, 1.0)
#define ASSERT_MATRIX_IS_NOT_UNITY(m) ASSERT_MATRIX_IS_NOT(m, 1.0, 0.0, 0.0, 1.0)

#define ASSERT_Z_VALUE(v, expected_value, expected_unit)\
    ASSERT_EQ_DBL(v.value(), expected_value)\
    ASSERT_EQ_PTR(v.unit(), expected_unit)

#define ASSERT_VALUE_TS(value, expected)\
    ASSERT_EQ_DBL(value.T, expected.T)\
    ASSERT_EQ_DBL(value.S, expected.S)

#define ASSERT_VALUE_T_S(value, expectedT, expectedS)\
    ASSERT_EQ_DBL(value.T, expectedT)\
    ASSERT_EQ_DBL(value.S, expectedS)

#define ASSERT_PTR_LIST(provided, expected)\
    ASSERT_EQ_INT(provided.size(), expected.size())\
    for (int i = 0; i < provided.size(); i++) {\
        ASSERT_EQ_PTR(provided.at(i), expected.at(i))\
    }

#define LOG_SCHEMA_FILE(file) {\
    auto report = file.report().str().trimmed(); \
    auto message = report.isEmpty()? "    (empty)": report;\
    test->logMessage("Load report:\n" % message % "\n");\
}

#define ASSERT_SCHEMA_STATE(expected_state)\
    ASSERT_EQ_INT(int(schema.state().current()), int(expected_state))

#define TAKE_ELEM_PTR(elem_var, index)\
    std::unique_ptr<Element> auto_##elem_var(schema.element(index));\
    auto elem_var = auto_##elem_var.get();\
    Q_UNUSED(elem_var)

#define ASSERT_ELEM_COUNT(expected_count)\
    ASSERT_EQ_INT(schema.count(), expected_count)

////////////////////////////////////////////////////////////////////////////////

#define ASSERT_ELEMENT(elem_index, expected_type, expected_label, expected_title, expected_disabled) { \
    TEST_LOG_SEPARATOR\
    TEST_LOG(QString("Element #%1").arg(elem_index))\
    ASSERT_IS_TRUE(elem_index < schema.elements().size())\
    elem = schema.element(elem_index);\
    TEST_LOG(elem->type())\
    ASSERT_IS_TYPE(elem, Elem ## expected_type)\
    TEST_LOG(elem->displayLabelTitle())\
    ASSERT_EQ_STR(elem->label(), expected_label)\
    ASSERT_EQ_STR(elem->title(), expected_title)\
    ASSERT_IS_TRUE(elem->disabled() == expected_disabled)\
}

//#define ASSERT_PARAM(param_name, expected_value, expected_unit) {
//    Z::Parameter* p;
//    ASSERT_IS_NOT_NULL(p = elem->params().byAlias(param_name))
//    TEST_LOG(p->str())
//    ASSERT_Z_VALUE(p->value(), expected_value, Z::Units::expected_unit())
//}

#endif // Z_TEST_UTILS_H
