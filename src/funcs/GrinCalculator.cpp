#include "GrinCalculator.h"

#include <cmath>

#include <QString>
#include <QDebug>

#define MAX_SOLVE_ITERATIONS 1000

using namespace std;

namespace GrinCalculator {

double calc_focus(double L, double n0, double n2)
{
    if (L < 0) return std::nan("");
    if (n0 < 0) return std::nan("");
    if (n2 < 0) return std::nan("");

    const double g = sqrt(n2/n0);
    return 1 / tan(g*L) / n0 / g;
}

double solve_n2(double L, double F, double n0)
{
    if (L < 0) return std::nan("");
    if (F < 0) return std::nan("");
    if (n0 < 0) return std::nan("");

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
            qWarning() << QString("Solving for n2 is not finished after %1 iterations. "
                                  "Parameters: L=%2, F=%3, n0=%4. Stopped at x_mid=%5 y_mid=%6")
                          .arg(MAX_SOLVE_ITERATIONS).arg(L, 0, 'g', 16).arg(F, 0, 'g', 16)
                          .arg(n0, 0, 'g', 16).arg(x_mid, 0, 'g', 16).arg(y_mid, 0, 'g', 16);
            break;
        }
    }
    double tmp = x_mid / L;
    return n0 * tmp * tmp;
}

} // namespace GrinCalculator
