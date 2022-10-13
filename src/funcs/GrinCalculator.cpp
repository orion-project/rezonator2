#include "GrinCalculator.h"

#include "../core/Protocol.h"

#include <cmath>

#include <QApplication>

#define MAX_SOLVE_ITERATIONS 1000

namespace GrinCalculator {

GrinResult check_params(const double& L, const double& n0)
{
    if (L < 0) return GrinResult::fail(qApp->translate("GrinCalculator", "L must be positive"));
    if (n0 < 0) return GrinResult::fail(qApp->translate("GrinCalculator", "n0 must be positive"));
    return GrinResult::ok(0);
}

GrinResult calc_focus(const double& L, const double& n0, const double& n2)
{
    auto res = check_params(L, n0);
    if (!res.ok()) return res;

    const double g = sqrt(qAbs(n2)/n0);
    const double tg = n2 < 0 ? -tanh(g*L) : tan(g*L);
    return GrinResult::ok(1 / tg / n0 / g);
}

// The algorithm is driven by the shape of equation
// see $PROJECT/calc/grin_focus_assessment.png
// and $PROJECT/calc/grin_focus_assessment.ipynb
GrinResult solve_n2(const double& L, const double& F, const double& n0)
{
    auto res = check_params(L, n0);
    if (!res.ok()) return res;

    // F = 1 / (tan(g*L) * n0 * g)
    // F * tan(g*L) * n0 * g = 1
    // F * tan(g*L) * n0 * g*L = L
    // F * tan(x) * n0 * x - L = 0
    auto equation = [&](const double& x) {
        return x * F * n0 * tan(x) - L;
    };

    double x_next = 0;
    double x_prev = 0;
    double y_next = equation(x_next);
    while (y_next < 0) {
        x_prev = x_next;
        x_next += 0.1;
        y_next = equation(x_next);
    }
    int safe_count = 0;
    double x_mid = x_next;
    double y_mid = y_next;
    while (abs(y_mid) > 1e-8) {
        x_mid = (x_prev + x_next) / 2.0;
        y_mid = equation(x_mid);
        if (y_next > 0 and y_mid < 0) {
            x_prev = x_mid;
        } else {
            x_next = x_mid;
            y_next = y_mid;
        }
        if (++safe_count >= MAX_SOLVE_ITERATIONS) {
            Z_WARNING(QString("Solving for n2 is not finished after %1 iterations. "
                              "Parameters: L=%2, F=%3, n0=%4. Stopped at x_mid=%5 y_mid=%6")
                      .arg(MAX_SOLVE_ITERATIONS).arg(L, 0, 'g', 16).arg(F, 0, 'g', 16)
                      .arg(n0, 0, 'g', 16).arg(x_mid, 0, 'g', 16).arg(y_mid, 0, 'g', 16))
            return GrinResult::fail("Failed to solve for n2 with given F");
        }
    }
    const double tmp = x_mid / L;
    return GrinResult::ok(n0 * tmp * tmp);
}

} // namespace GrinCalculator
