#ifndef Z_TEST_UTILS_H
#define Z_TEST_UTILS_H

#include "../core/Values.h"
#include "testing/OriTestBase.h"

//------------------------------------------------------------------------------
//                       Application specific asserts
//------------------------------------------------------------------------------

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

#define ASSERT_EQ_MATRIX(m, expected_m)\
    ASSERT_EQ_DBL((m).A, (expected_m).A); \
    ASSERT_EQ_DBL((m).B, (expected_m).B); \
    ASSERT_EQ_DBL((m).C, (expected_m).C); \
    ASSERT_EQ_DBL((m).D, (expected_m).D);

#define ASSERT_MATRIX_IS(m, a, b, c, d) \
    ASSERT_EQ_DBL((m).A, a); \
    ASSERT_EQ_DBL((m).B, b); \
    ASSERT_EQ_DBL((m).C, c); \
    ASSERT_EQ_DBL((m).D, d);

#define ASSERT_MATRIX_NEAR(m, a, b, c, d, eps) \
    ASSERT_NEAR_DBL((m).A, a, eps); \
    ASSERT_NEAR_DBL((m).B, b, eps); \
    ASSERT_NEAR_DBL((m).C, c, eps); \
    ASSERT_NEAR_DBL((m).D, d, eps);

#define ASSERT_MATRIX_IS_UNITY(m) ASSERT_MATRIX_IS(m, 1.0, 0.0, 0.0, 1.0)

//------------------------------------------------------------------------------
//                       Definitions for schema readers
//------------------------------------------------------------------------------

#ifdef Q_OS_MAC
// Lock near the application bundle, if file is not found near the executable
#define TEST_FILE(var, file_name)\
    QString var = qApp->applicationDirPath() % "/test_files/" % file_name;\
    if (!QFile::exists(var)) {\
        var = qApp->applicationDirPath() % "/../../../test_files/" % file_name;\
        if (!QFile::exists(var)) ASSERT_FAIL("File does not exist: " + var)\
    }
#else
#define TEST_FILE(var, file_name)\
    QString var = qApp->applicationDirPath() % "/test_files/" % file_name;\
    if (!QFile::exists(var)) ASSERT_FAIL("File does not exist: " + var)
#endif

#define LOG_SCHEMA_READER(reader) {\
    auto report = reader.report().str().trimmed(); \
    auto message = report.isEmpty()? "    (empty)": report;\
    test->logMessage("Loading report:\n" % message % "\n");\
}

#endif // Z_TEST_UTILS_H
