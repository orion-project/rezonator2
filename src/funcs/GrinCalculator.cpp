#include "GrinCalculator.h"

#include <cmath>

using namespace std;

namespace GrinCalculator {

double calc_focus(double L, double n0, double n2)
{
    const double g = sqrt(n2/n0);
    return 1 / tan(g*L) / n0 / g;
}

double solve_n2(double L, double F, double n0)
{
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
    }
    double tmp = x_mid / L;
    return n0 * tmp * tmp;
}

} // namespace GrinCalculator
