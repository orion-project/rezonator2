#ifndef Z_TEST_UTILS_H
#define Z_TEST_UTILS_H

#define ASSERT_UNIT(unit_given, unit_expected) { \
    if (unit_given != unit_expected) \
    { \
        test->setResult(false); \
        test->setMessage("Unit is not equal to expected" ); \
        test->logAssertion("ARE UNITS EQUAL", \
                           QString("%1 == %2").arg(#unit_given).arg(#unit_expected), \
                           unit_expected->alias(), \
                           unit_given->alias(), \
                           __FILE__, __LINE__); \
        return; \
    }}

#define ASSERT_MATRIX_IS(m, a, b, c, d) \
    ASSERT_EQ_DBL((m).A, a); \
    ASSERT_EQ_DBL((m).B, b); \
    ASSERT_EQ_DBL((m).C, c); \
    ASSERT_EQ_DBL((m).D, d);

#define ASSERT_MATRIX_IS_UNITY(m) ASSERT_MATRIX_IS(m, 1.0, 0.0, 0.0, 1.0)

#define ASSERT_Z_VALUE_AND_UNIT(v, expected_value, expected_unit)\
    ASSERT_EQ_DBL(v.value(), expected_value)\
    ASSERT_UNIT(v.unit(), expected_unit)

#endif // Z_TEST_UTILS_H
