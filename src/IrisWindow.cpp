#include "IrisWindow.h"

#include "CustomPrefs.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriWindows.h"

#include <cmath>

namespace {

IrisWindow* __instance = nullptr;

class Glass
{
public:
    const QString& title() const { return _title; }
    const QString& comment() const { return _comment; }
    double lambdaMin() const { return _lambdaMin; }
    double lambdaMax() const { return _lambdaMax; }

    virtual double index0(const double& l) const = 0;

    virtual double index1(const double& l) const
    {
        // by two points
        //return (index0(l+_deltaX) - index0(l-_deltaX)) / 2.0 / _deltaX;

        // by four points
        return (-index0(l + 1.5 * _dl) + 27*index0(l + 0.5 * _dl) -
            27*index0(l - 0.5 * _dl) + index0(l - 1.5 * _dl)
            ) / 24.0 / _dl;
    }

    virtual double index2(const double& l) const
    {
        // by three points
        //return (index0(l+_dl) - 2*index0(l) + index0(l-_dl)) / _dl / _dl;

        // by five points
        return (-index0(l + 2*_dl) + 16*index0(l + _dl) -
            30*index0(l) + 16*index0(l - _dl) - index0(l - 2*_dl)
            ) /12.0 / _dl / _dl;
    }

protected:
    double _lambdaMin, _lambdaMax, _dl;
    QString _title, _comment;
    QVector<double> _coeffs;

    virtual void read(QJsonObject& obj) = 0;
    virtual void write(QJsonObject& obj) = 0;
};

class GlassShott : public Glass
{
public:
    double index0(const double& l) const override
    {
        const double l2 = l * l;
        const double l4 = l2 * l2;
        const double l6 = l4 * l2;
        const double l8 = l4 * l4;
        return sqrt(
            _coeffs[0] +
            _coeffs[1] * l2 +
            _coeffs[2] / l2 +
            _coeffs[3] / l4 +
            _coeffs[4] / l6 +
            _coeffs[5] / l8);
    }

    double index1(const double& l) const override
    {
        const double l2 = l * l;
        const double l4 = l2 * l2;
        const double l6 = l4 * l2;
        const double l8 = l4 * l4;
        return (
            _coeffs[1] * l -
            _coeffs[2] / l2 / l -
            _coeffs[3] * 2 / l4 / l -
            _coeffs[4] * 3 / l6 / l -
            _coeffs[5] * 4 / l8 / l ) /
          sqrt(
            _coeffs[0] +
            _coeffs[1] * l2 +
            _coeffs[2] / l2 +
            _coeffs[3] / l4 +
            _coeffs[4] / l6 +
            _coeffs[5] / l8);
    }

    double index2(const double& l) const override
    {
        const double l2 = l * l;
        const double l4 = l2 * l2;
        const double l6 = l4 * l2;
        const double l8 = l4 * l4;
        const double n = sqrt(
            _coeffs[0] +
            _coeffs[1] * l2 +
            _coeffs[2] / l2 +
            _coeffs[3] / l4 +
            _coeffs[4] / l6 +
            _coeffs[5] / l8);
        const double t =
            _coeffs[1] * l -
            _coeffs[2] / l2 / l -
            _coeffs[3] *2 / l4 / l -
            _coeffs[4] *3 / l6 / l -
            _coeffs[5] *4 / l8 / l;
        return -t*t/n/n/n + (
            _coeffs[1] +
            _coeffs[2] *3  / l4 +
            _coeffs[3] *10 / l6 +
            _coeffs[4] *21 / l8 +
            _coeffs[5] *36 / l8 / l2) / n;
    }
};

class GlassSellmeier : public Glass
{
public:
    double index0(const double& l) const override
    {
        const double l2 = l * l;
        return sqrt(1 +
           _coeffs[0] * l2 / (l2 - _coeffs[3]) +
           _coeffs[1] * l2 / (l2 - _coeffs[4]) +
           _coeffs[2] * l2 / (l2 - _coeffs[5]));
    }

    double index1(const double& l) const override
    {
        const double l2 = l*l;
        const double b1 = _coeffs[0] * l;
        const double b2 = _coeffs[1] * l;
        const double b3 = _coeffs[2] * l;
        const double c1 = l2 - _coeffs[3];
        const double c2 = l2 - _coeffs[4];
        const double c3 = l2 - _coeffs[5];
        return (
              b1  / c1 * (1 - l2 / c1) +
              b2  / c2 * (1 - l2 / c2) +
              b3  / c3 * (1 - l2 / c3)) /
          sqrt(1 +
               l * b1 / c1 +
               l * b2 / c2 +
               l * b3 / c3);
    }

    double index2(const double& l) const override
    {
        const double l2 = l * l;
        const double b1 = l2 * _coeffs[0];
        const double b2 = l2 * _coeffs[1];
        const double b3 = l2 * _coeffs[2];
        const double c1 = l2 - _coeffs[3];
        const double c2 = l2 - _coeffs[4];
        const double c3 = l2 - _coeffs[5];
        const double n = sqrt(1 + b1/c1 + b2/c2 + b3/c3);
        const double t =
            _coeffs[0]*l/c1 - b1*l/c1/c1 +
            _coeffs[1]*l/c2 - b2*l/c2/c2 +
            _coeffs[2]*l/c3 - b3*l/c3/c3;
        return -t*t/n/n/n + (
            _coeffs[0]/c1 - 5*b1/c1/c1 + 4*b1*l2/c1/c1/c1 +
            _coeffs[1]/c2 - 5*b2/c2/c2 + 4*b2*l2/c2/c2/c2 +
            _coeffs[2]/c3 - 5*b3/c3/c3 + 4*b3*l2/c3/c3/c3)/n;
    }
};

class GlassReznik : public Glass
{
public:
    double index0(const double& l) const override
    {
        const double deltal_reznik = (_lambdaMax * _lambdaMax - _lambdaMin * _lambdaMin) / 2.0;
        const double lav_reznik = (_lambdaMax * _lambdaMax + _lambdaMin * _lambdaMin) / 2.0;
        const double nu = (l*l - lav_reznik) / deltal_reznik;
        const double Lmax = 1.0 / (_lambdaMax * _lambdaMax - _coeffs[0]);
        const double Lmin = 1.0 / (_lambdaMin * _lambdaMin - _coeffs[0]);
        const double DL = (Lmin - Lmax) / 2.0;
        const double Lav = (Lmin + Lmax) / 2.0;
        const double Ll = 1.0 / (l*l - _coeffs[0]);
        const double mu = (Ll - Lav) / DL;
        return
            _coeffs[1] +
            _coeffs[3] * nu +
            _coeffs[5] * nu * nu +
            _coeffs[7] * nu * nu * nu +
            _coeffs[9] * nu * nu * nu * nu +
            _coeffs[2] * mu +
            _coeffs[4] * mu * mu +
            _coeffs[6] * mu * mu * mu +
            _coeffs[8] * mu * mu * mu * mu +
            _coeffs[10] * mu * mu * mu * mu * mu;
    }
};

//class GlassCustom : public Glass
//{
//public:
//    double calcIndex(const double& lambda) const override
//    {
//        return 0;
//    }

//protected:
//    void read(QJsonObject& obj) override
//    {

//    }

//    void write(QJsonObject& obj) override
//    {

//    }

//private:
//    QString _formula;
//};

} // namespace

void IrisWindow::showWindow()
{
    if (!__instance)
        __instance = new IrisWindow;
    __instance->show();
    __instance->activateWindow();
}

IrisWindow::IrisWindow(QWidget *parent) : QWidget(parent)
{
    Ori::Wnd::initWindow(this, "Iris", ":/window_icons/iris");

    Ori::Layouts::LayoutV({}).setSpacing(0).setMargin(0).useFor(this);

    restoreState();
    Ori::Wnd::moveToScreenCenter(this);
}

IrisWindow::~IrisWindow()
{
    storeState();
    __instance = nullptr;
}

void IrisWindow::restoreState()
{
    QJsonObject root = CustomDataHelpers::loadCustomData("iris");

    CustomDataHelpers::restoreWindowSize(root, this, 700, 500);
}

void IrisWindow::storeState()
{
    QJsonObject root;

    CustomDataHelpers::storeWindowSize(root, this);

    CustomDataHelpers::saveCustomData(root, "iris");
}
