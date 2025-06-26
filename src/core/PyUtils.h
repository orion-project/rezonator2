#ifndef PY_UTILS_H
#define PY_UTILS_H

#include "Python.h"

#define STOP_MODULE_INIT { \
    qCritical() << Q_FUNC_INFO << "Failure"; \
    return -1; \
}

#endif // PY_UTILS_H
