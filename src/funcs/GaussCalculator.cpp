#include "GaussCalculator.h"

#include <complex>

#include <QDebug>

#define _PI_ 3.14159265358979323846

void GaussCalculator::calc()
{
    switch (_ref)
    {
    case Ref::Lambda:
        if (_lock == Lock::Waist)
        {
            calc_Z0_from_W0_M2();
            calc_Vs_from_W0_M2();
            calc_W_from_W0_Z_Z0();
            calc_R_from_Z_Z0();
            calc_Q1_from_W_R();
            calc_Q_from_Q1();
        }
        else
        {
            calc_Z0_from_R_Z();
            calc_W0_from_W_Z_Z0();
            calc_M2_from_W0_Z0();
            calc_Vs_from_W0_M2();
        }
        break;

    case Ref::W0:
        if (_lock == Lock::Waist)
        {
            calc_Z0_from_W0_M2();
            calc_Vs_from_W0_M2();
            calc_W_from_W0_Z_Z0();
            calc_R_from_Z_Z0();
            calc_Q1_from_W_R();
            calc_Q_from_Q1();
        }
        else
        {
            calc_Z_from_R_W_W0();
            calc_Z0_from_R_Z();
            calc_M2_from_W0_Z0();
            calc_Vs_from_W0_M2();
        }
        break;

    case Ref::Z0:
        if (_lock == Lock::Waist)
        {
            calc_M2_from_W0_Z0();
            calc_Vs_from_W0_M2();
            calc_W_from_W0_Z_Z0();
            calc_R_from_Z_Z0();
            calc_Q1_from_W_R();
            calc_Q_from_Q1();
        }
        else
        {
            qDebug() << "555555555";
            calc_Z_from_R_Z0();
            calc_W0_from_W_Z_Z0();
            calc_M2_from_W0_Z0();
            calc_Vs_from_W0_M2();
        }
        break;

    case Ref::Vs:
        if (_lock == Lock::Waist)
        {
            calc_M2_from_W0_Vs();
            calc_Z0_from_W0_M2();
            calc_W_from_W0_Z_Z0();
            calc_R_from_Z_Z0();
            calc_Q1_from_W_R();
            calc_Q_from_Q1();
        }
        else
        {
            calc_Z_from_W_R_Vs();
            calc_Z0_from_R_Z();
            calc_W0_from_W_Z_Z0();
            calc_M2_from_W0_Z0();
        }

    case Ref::M2:
        if (_lock == Lock::Waist)
        {
            calc_Z0_from_W0_M2();
            calc_Vs_from_W0_M2();
            calc_W_from_W0_Z_Z0();
            calc_R_from_Z_Z0();
            calc_Q1_from_W_R();
            calc_Q_from_Q1();
        }
        else
        {
            calc_Z0_from_R_Z();
            calc_W0_from_W_Z_Z0();
            calc_M2_from_W0_Z0();
            calc_Vs_from_W0_M2();
        }
        break;

    case Ref::Z:
        if (_lock == Lock::Waist)
        {
            calc_Z0_from_W0_M2();
            calc_Vs_from_W0_M2();
            calc_W_from_W0_Z_Z0();
            calc_R_from_Z_Z0();
            calc_Q1_from_W_R();
            calc_Q_from_Q1();
        }
        else
        {
            calc_Z0_from_R_Z();
            calc_W0_from_W_Z_Z0();
            calc_M2_from_W0_Z0();
            calc_Vs_from_W0_M2();
        }
        break;

    case Ref::W:
        if (_lock == Lock::Waist)
        {
            calc_Z0_from_W0_W_Z();
            calc_M2_from_W0_Z0();
            calc_Vs_from_W0_M2();
            calc_R_from_Z_Z0();
        }
        else
        {
            calc_Z0_from_R_Z();
            calc_W0_from_W_Z_Z0();
            calc_M2_from_W0_Z0();
            calc_Vs_from_W0_M2();
        }
        calc_Q1_from_W_R();
        calc_Q_from_Q1();
        break;

    case Ref::R:
        calc_Z0_from_R_Z();
        if (_lock == Lock::Waist)
        {
            calc_M2_from_W0_Z0();
            calc_Vs_from_W0_M2();
            calc_W_from_W0_Z_Z0();
        }
        else
        {
            calc_W0_from_W_Z_Z0();
            calc_M2_from_W0_Z0();
            calc_Vs_from_W0_M2();
        }
        calc_Q1_from_W_R();
        calc_Q_from_Q1();
        break;

    case Ref::Q:
        calc_Q1_from_Q();
        calc_W_from_Q1();
        calc_R_from_Q1();
        if (_lock == Lock::Waist)
        {
            calc_Z_from_R_W_W0();
            calc_Z0_from_R_Z();
        }
        else
        {
            calc_Z0_from_R_Z();
            calc_W0_from_W_Z_Z0();
        }
        calc_M2_from_W0_Z0();
        calc_Vs_from_W0_M2();
        break;

    case Ref::Q1Re:
        calc_R_from_Q1();
        _ref = Ref::R;
        calc();
        break;

    case Ref::Q1Im:
        calc_W_from_Q1();
        _ref = Ref::W;
        calc();
        break;
    }
}

void GaussCalculator::calc_Vs_from_W0_M2()
{
    _Vs = _M2 * _lambda / _PI_ / _w0;
}

void GaussCalculator::calc_W_from_W0_Z_Z0()
{
    _w = _w0 * sqrt(1 + _z*_z / (_z0*_z0));
}

void GaussCalculator::calc_R_from_Z_Z0()
{
    _R = _z * (1 + _z0*_z0 / (_z*_z));
}

void GaussCalculator::calc_Q1_from_W_R()
{
    _re_q1 = 1 / _R;
    _im_q1 = _lambda / _PI_ / (_w*_w);
}

void GaussCalculator::calc_Q_from_Q1()
{
    std::complex<double> Q1(_re_q1, _im_q1);
    std::complex<double> Q = 1.0 / Q1;
    _re_q = Q.real();
    _im_q = Q.imag();
}

void GaussCalculator::calc_Q1_from_Q()
{
    std::complex<double> Q(_re_q1, _im_q1);
    std::complex<double> Q1 = 1.0 / Q;
    _re_q1 = Q1.real();
    _im_q1 = Q1.imag();
}

void GaussCalculator::calc_W_from_Q1()
{
    _w = sqrt(_lambda / _PI_ / _im_q1);
}

void GaussCalculator::calc_R_from_Q1()
{
    _R = 1.0 / _re_q1;
}

void GaussCalculator::calc_M2_from_W0_Z0()
{
    _M2 = _PI_ * (_w0*_w0) / _lambda / _z0;
}

void GaussCalculator::calc_M2_from_W0_Vs()
{
    _M2 = _PI_ * _w0 * _Vs / _lambda;
}

void GaussCalculator::calc_Z0_from_W0_M2()
{
    _z0 = _PI_ * (_w0*_w0) / _lambda / _M2;
}

void GaussCalculator::calc_Z0_from_R_Z()
{
    _z0 = sqrt(_z * (_R - _z));
}

void GaussCalculator::calc_Z0_from_W0_W_Z()
{
    _z0 = sqrt(_z*_z * _w0*_w0 / (_w*_w - _w0*_w0));
}

void GaussCalculator::calc_W0_from_W_Z_Z0()
{
    _w0 = _w / sqrt(1 + _z*_z / (_z0*_z0));
    qDebug() << "_w0" << _w0 << "_w" << _w << "_z" << _z << "_z0" << _z0;
}

void GaussCalculator::calc_W0_from_Z0_M2()
{
    _w0 = sqrt(_z0 * _lambda * _M2 / _PI_);
}

void GaussCalculator::calc_Z_from_R_Z0()
{
    qDebug() << "far" << (_R + sqrt(_R*_R - 4*_z0*_z0)) / 2.0;
    qDebug() << "near" << (_R - sqrt(_R*_R - 4*_z0*_z0)) / 2.0;
    if (_zone == Zone::Far)
        _z = (_R + sqrt(_R*_R - 4*_z0*_z0)) / 2.0;
    else
        _z = (_R - sqrt(_R*_R - 4*_z0*_z0)) / 2.0;
    qDebug() << "_z" << _z;
}

void GaussCalculator::calc_Z_from_R_W_W0()
{
    _z = _R * (1.0 - _w0*_w0 / (_w*_w));
}

void GaussCalculator::calc_Z_from_W_R_Vs()
{
    _z = _w*_w / _R / (_Vs*_Vs);
}
