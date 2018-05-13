#ifndef Z_TEST_UTILS_H
#define Z_TEST_UTILS_H

#include "../core/Values.h"

#define Z_VALUE_LITERAL(unit)\
    inline Z::Value operator "" _##unit(long double value) { return Z::Value(value, Z::Units::unit()); }\
    inline Z::Value operator "" _##unit(unsigned long long value) { return Z::Value(value, Z::Units::unit()); }

Z_VALUE_LITERAL(Ao)
Z_VALUE_LITERAL(mkm)
Z_VALUE_LITERAL(mm)
Z_VALUE_LITERAL(cm)
Z_VALUE_LITERAL(m)

#define ASSERT_EQ_UNIT(unit_given, unit_expected) { \
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

#define ASSERT_EQ_ZVALUE(v, expected_value)\
    ASSERT_EQ_DBL(v.value(), expected_value.value())\
    ASSERT_EQ_UNIT(v.unit(), expected_value.unit())

#define ASSERT_MATRIX_IS(m, a, b, c, d) \
    ASSERT_EQ_DBL((m).A, a); \
    ASSERT_EQ_DBL((m).B, b); \
    ASSERT_EQ_DBL((m).C, c); \
    ASSERT_EQ_DBL((m).D, d);

#define ASSERT_MATRIX_IS_UNITY(m) ASSERT_MATRIX_IS(m, 1.0, 0.0, 0.0, 1.0)

#endif // Z_TEST_UTILS_H
