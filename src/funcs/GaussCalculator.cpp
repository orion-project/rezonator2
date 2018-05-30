#include "GaussCalculator.h"

#include <complex>

//#define LOG_STAGES

#ifdef LOG_STAGES
#include <QDebug>
#define LOG_REF(var, lock)\
    qDebug() << "Ref::"#var << "Lock::"#lock;
#define LOG_VARS(method, ...)\
    int index = 0;\
    QStringList report;\
    QVector<double> values({__VA_ARGS__});\
    for (const QString& name : QString(#__VA_ARGS__).split(','))\
        report << QString("%1 = %2").arg(name.trimmed()).arg(values.at(index++));\
    qDebug() << method << report.join(' ');
#else
#define LOG_REF(var, lock)
#define LOG_VARS(method, ...)
#endif

GaussCalculator::GaussCalculator() :
    _MI(1), _z0(NAN), _w0(NAN), _Vs(NAN), _z(NAN), _w(NAN), _R(NAN),
    _re_q(NAN), _im_q(NAN), _re_q1(NAN), _im_q1(NAN), _lambda(NAN)
{
}

void GaussCalculator::calc()
{
    switch (_ref)
    {
    case Ref::Lambda:
        if (_lock == Lock::Waist)
        {
            LOG_REF(Lambda, Waist)
            calc_Z0_from_W0_MI();
        }
        else
        {
            LOG_REF(Lambda, Front)
            calc_Z0_from_R_Z();
            calc_W0_from_W_Z_Z0();
            calc_MI_from_W0_Z0();
        }
        calc_Vs_from_W0_MI();
        calc_W_from_W0_Z_Z0();
        calc_R_from_Z_Z0();
        calc_Q1_from_W_R();
        calc_Q_from_Q1();
        break;

    case Ref::W0:
        if (_lock == Lock::Waist)
        {
            LOG_REF(W0, Waist)
            calc_Z0_from_W0_MI();
        }
        else
        {
            LOG_REF(W0, Front)
            calc_Z_from_R_W_W0();
            calc_Z0_from_R_Z();
            calc_MI_from_W0_Z0();
        }
        calc_Vs_from_W0_MI();
        calc_W_from_W0_Z_Z0();
        calc_R_from_Z_Z0();
        calc_Q1_from_W_R();
        calc_Q_from_Q1();
        break;

    case Ref::Z0:
        if (_lock == Lock::Waist)
        {
            LOG_REF(Z0, Waist)
        }
        else
        {
            LOG_REF(Z0, Front)
            calc_Z_from_R_Z0();
            calc_W0_from_W_Z_Z0();
        }
        calc_MI_from_W0_Z0();
        calc_Vs_from_W0_MI();
        calc_W_from_W0_Z_Z0();
        calc_R_from_Z_Z0();
        calc_Q1_from_W_R();
        calc_Q_from_Q1();
        break;

    case Ref::Vs:
        if (_lock == Lock::Waist)
        {
            LOG_REF(Vs, Waist)
            calc_MI_from_W0_Vs();
            calc_Z0_from_W0_MI();
        }
        else
        {
            LOG_REF(Vs, Front)
            calc_Z_from_W_R_Vs();
            calc_Z0_from_R_Z();
            calc_W0_from_W_Z_Z0();
            calc_MI_from_W0_Z0();
        }
        calc_W_from_W0_Z_Z0();
        calc_R_from_Z_Z0();
        calc_Q1_from_W_R();
        calc_Q_from_Q1();
        break;

    case Ref::MI:
        if (_lock == Lock::Waist)
        {
            LOG_REF(MI, Waist)
            calc_Z0_from_W0_MI();
        }
        else
        {
            LOG_REF(MI, Front)
            calc_Z_from_R_W_MI();
            calc_Z0_from_R_Z();
            calc_W0_from_Z0_MI();
        }
        calc_Vs_from_W0_MI();
        calc_W_from_W0_Z_Z0();
        calc_R_from_Z_Z0();
        calc_Q1_from_W_R();
        calc_Q_from_Q1();
        break;

    case Ref::Z:
        if (_lock == Lock::Waist)
        {
            LOG_REF(Z, Waist)
            calc_Z0_from_W0_MI();
        }
        else
        {
            LOG_REF(Z, Front)
            calc_Z0_from_R_Z();
            calc_W0_from_W_Z_Z0();
            calc_MI_from_W0_Z0();
        }
        calc_Vs_from_W0_MI();
        calc_W_from_W0_Z_Z0();
        calc_R_from_Z_Z0();
        calc_Q1_from_W_R();
        calc_Q_from_Q1();
        break;

    case Ref::W:
        if (_lock == Lock::Waist)
        {
            LOG_REF(W, Waist)
            calc_Z0_from_W0_W_Z();
            calc_MI_from_W0_Z0();
            calc_Vs_from_W0_MI();
        }
        else
        {
            LOG_REF(W, Front)
            calc_Z0_from_R_Z();
            calc_W0_from_W_Z_Z0();
            calc_MI_from_W0_Z0();
            calc_Vs_from_W0_MI();
        }
        calc_R_from_Z_Z0();
        calc_Q1_from_W_R();
        calc_Q_from_Q1();
        break;

    case Ref::R:
        if (_lock == Lock::Waist)
        {
            LOG_REF(R, Waist)
            calc_Z0_from_R_Z();
            calc_MI_from_W0_Z0();
            calc_Vs_from_W0_MI();
        }
        else
        {
            LOG_REF(R, Front)
            calc_Z0_from_R_Z();
            calc_W0_from_W_Z_Z0();
            calc_MI_from_W0_Z0();
            calc_Vs_from_W0_MI();
        }
        calc_W_from_W0_Z_Z0();
        calc_Q1_from_W_R();
        calc_Q_from_Q1();
        break;

    case Ref::Q:
        if (_lock == Lock::Waist)
        {
            LOG_REF(Q, Waist)
            calc_Q1_from_Q();
            calc_W_from_Q1();
            calc_R_from_Q1();
            calc_Z_from_R_W_W0();
        }
        else
        {
            LOG_REF(Q, Front)
            calc_Q1_from_Q();
            calc_W_from_Q1();
            calc_R_from_Q1();
        }
        calc_Z0_from_R_Z();
        calc_W0_from_W_Z_Z0();
        calc_MI_from_W0_Z0();
        calc_Vs_from_W0_MI();
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

void GaussCalculator::calc_Vs_from_W0_MI()
{
    _Vs = _MI * _lambda / M_PI / _w0;
    LOG_VARS("calc_Vs_from_W0_M2", _MI, _lambda, _w0, _Vs)
}

void GaussCalculator::calc_W_from_W0_Z_Z0()
{
    _w = _w0 * sqrt(1 + _z*_z / (_z0*_z0));
    LOG_VARS("calc_W_from_W0_Z_Z0", _w0, _z, _z0, _w)
}

void GaussCalculator::calc_R_from_Z_Z0()
{
    _R = _z * (1 + _z0*_z0 / (_z*_z));
    LOG_VARS("calc_R_from_Z_Z0", _z, _z0, _R)
}

void GaussCalculator::calc_Q1_from_W_R()
{
    _re_q1 = 1 / _R;
    _im_q1 = _lambda / M_PI / (_w*_w);
    LOG_VARS("calc_Q1_from_W_R", _R, _lambda, _w, _re_q1, _im_q1)
}

void GaussCalculator::calc_Q_from_Q1()
{
    std::complex<double> Q1(_re_q1, _im_q1);
    std::complex<double> Q = 1.0 / Q1;
    _re_q = Q.real();
    _im_q = Q.imag();
    LOG_VARS("calc_Q_from_Q1", _re_q1, _im_q1, _re_q, _im_q)
}

void GaussCalculator::calc_Q1_from_Q()
{
    std::complex<double> Q(_re_q, _im_q);
    std::complex<double> Q1 = 1.0 / Q;
    _re_q1 = Q1.real();
    _im_q1 = Q1.imag();
    LOG_VARS("calc_Q1_from_Q", _re_q, _im_q, _re_q1, _im_q1)
}

void GaussCalculator::calc_W_from_Q1()
{
    _w = sqrt(_lambda / M_PI / _im_q1);
    LOG_VARS("calc_W_from_Q1", _lambda, _im_q1, _w)
}

void GaussCalculator::calc_R_from_Q1()
{
    _R = 1.0 / _re_q1;
    LOG_VARS("calc_R_from_Q1", _re_q1, _R)
}

void GaussCalculator::calc_MI_from_W0_Z0()
{
    _MI = M_PI * (_w0*_w0) / _lambda / _z0;
    LOG_VARS("calc_M2_from_W0_Z0", _w0, _lambda, _z0, _MI)
}

void GaussCalculator::calc_MI_from_W0_Vs()
{
    _MI = M_PI * _w0 * _Vs / _lambda;
    LOG_VARS("calc_M2_from_W0_Vs", _w0, _lambda, _Vs, _MI)
}

void GaussCalculator::calc_Z0_from_W0_MI()
{
    _z0 = M_PI * (_w0*_w0) / _lambda / _MI;
    LOG_VARS("calc_Z0_from_W0_M2", _w0, _lambda, _MI, _z0)
}

void GaussCalculator::calc_Z0_from_R_Z()
{
    _z0 = sqrt(_z * (_R - _z));
    LOG_VARS("calc_Z0_from_R_Z", _R, _z, _z0)
}

void GaussCalculator::calc_Z0_from_W0_W_Z()
{
    _z0 = sqrt(_z*_z * _w0*_w0 / (_w*_w - _w0*_w0));
    LOG_VARS("calc_Z0_from_W0_W_Z", _w0, _w, _z, _z0)
}

void GaussCalculator::calc_W0_from_W_Z_Z0()
{
    _w0 = _w / sqrt(1 + _z*_z / (_z0*_z0));
    LOG_VARS("calc_W0_from_W_Z_Z0", _w, _z, _z0, _w0)
}

void GaussCalculator::calc_W0_from_Z0_MI()
{
    _w0 = sqrt(_z0 * _lambda * _MI / M_PI);
    LOG_VARS("calc_W0_from_Z0_M2", _z0, _lambda, _MI, _w0)
}

void GaussCalculator::calc_Z_from_R_Z0()
{
    if (_zone == Zone::Far)
    {
        _z = (_R + sqrt(_R*_R - 4*_z0*_z0)) / 2.0;
        LOG_VARS("calc_Z_from_R_Z0 (far)", _R, _z0, _z)
    }
    else
    {
        _z = (_R - sqrt(_R*_R - 4*_z0*_z0)) / 2.0;
        LOG_VARS("calc_Z_from_R_Z0 (near)", _R, _z0, _z)
    }
}

void GaussCalculator::calc_Z_from_R_W_W0()
{
    _z = _R * (1.0 - _w0*_w0 / (_w*_w));
    LOG_VARS("calc_Z_from_R_W_W0", _R, _w0, _w, _z)
}

void GaussCalculator::calc_Z_from_W_R_Vs()
{
    _z = _w*_w / _R / (_Vs*_Vs);
    LOG_VARS("calc_Z_from_W_R_Vs", _w, _R, _Vs, _z)
}

void GaussCalculator::calc_Z_from_R_W_MI()
{
    double tmp = _w*_w*_w*_w * M_PI*M_PI;
    _z = tmp * _R / (_MI*_MI * _lambda*_lambda * _R*_R + tmp);
    LOG_VARS("calc_Z_from_R_W_M2", _w, _R, _MI, _lambda, _z)
}
