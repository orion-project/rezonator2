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
    ASSERT_EQ_DBL((m).A.real(), (expected_m).A.real()); \
    ASSERT_EQ_DBL((m).B.real(), (expected_m).B.real()); \
    ASSERT_EQ_DBL((m).C.real(), (expected_m).C.real()); \
    ASSERT_EQ_DBL((m).D.real(), (expected_m).D.real()); \
    ASSERT_EQ_DBL((m).A.imag(), (expected_m).A.imag()); \
    ASSERT_EQ_DBL((m).B.imag(), (expected_m).B.imag()); \
    ASSERT_EQ_DBL((m).C.imag(), (expected_m).C.imag()); \
    ASSERT_EQ_DBL((m).D.imag(), (expected_m).D.imag());

#define ASSERT_NEAR_MATRIX(m, expected_m, eps)\
    ASSERT_NEAR_DBL((m).A.real(), (expected_m).A.real(), eps); \
    ASSERT_NEAR_DBL((m).B.real(), (expected_m).B.real(), eps); \
    ASSERT_NEAR_DBL((m).C.real(), (expected_m).C.real(), eps); \
    ASSERT_NEAR_DBL((m).D.real(), (expected_m).D.real(), eps); \
    ASSERT_NEAR_DBL((m).A.imag(), (expected_m).A.imag(), eps); \
    ASSERT_NEAR_DBL((m).B.imag(), (expected_m).B.imag(), eps); \
    ASSERT_NEAR_DBL((m).C.imag(), (expected_m).C.imag(), eps); \
    ASSERT_NEAR_DBL((m).D.imag(), (expected_m).D.imag(), eps);

#define ASSERT_MATRIX_IS(m, a, b, c, d) \
    ASSERT_EQ_DBL((m).A.real(), a); \
    ASSERT_EQ_DBL((m).B.real(), b); \
    ASSERT_EQ_DBL((m).C.real(), c); \
    ASSERT_EQ_DBL((m).D.real(), d); \
    ASSERT_EQ_DBL((m).A.imag(), 0); \
    ASSERT_EQ_DBL((m).B.imag(), 0); \
    ASSERT_EQ_DBL((m).C.imag(), 0); \
    ASSERT_EQ_DBL((m).D.imag(), 0);

#define ASSERT_MATRIX_NEAR(m, a, b, c, d, eps) \
    ASSERT_NEAR_DBL((m).A.real(), a, eps); \
    ASSERT_NEAR_DBL((m).B.real(), b, eps); \
    ASSERT_NEAR_DBL((m).C.real(), c, eps); \
    ASSERT_NEAR_DBL((m).D.real(), d, eps); \
    ASSERT_EQ_DBL((m).A.imag(), 0); \
    ASSERT_EQ_DBL((m).B.imag(), 0); \
    ASSERT_EQ_DBL((m).C.imag(), 0); \
    ASSERT_EQ_DBL((m).D.imag(), 0);

#define ASSERT_MATRIX_IS_UNITY(m) ASSERT_MATRIX_IS(m, 1.0, 0.0, 0.0, 1.0)

//------------------------------------------------------------------------------
//                       Definitions for schema readers
//------------------------------------------------------------------------------

#ifdef Q_OS_MAC
// Look near the application bundle, if file is not found near the executable
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
