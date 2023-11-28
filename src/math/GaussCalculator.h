#ifndef GAUSS_CALCULATOR_H
#define GAUSS_CALCULATOR_H

/**
  The tool for computing of Gaussian beam parameters.

  Computes some free parameters of Gaussian beam,
  when changing one of its parameter and fixing some others.

  The tool doesn't use units of measurement, all calculation are carried out in SI units.
  A tool's client (e.g. some window) is in charge for conversion to/from custom units.

  Usage scenario:
  - Make BeamCalculator object
  - Assign lock mode (it is `Waist` by default)
  - Assign fixed paramaters (depending on lock mode)
  - Call method `calc()`
  - All parameters can be used.
*/
class GaussCalculator
{
public:
    enum class Lock {Waist, Front};
    enum class Zone {Near, Far};
    enum class Ref {Lambda, W0, Z0, Vs, MI, Z, W, R, Q, Q1Im, Q1Re};

    GaussCalculator();

    void calc();

    double lambda() const { return _lambda; }
    double w0() const { return _w0; }
    double z0() const { return _z0; }
    double Vs() const { return _Vs; }
    double MI() const { return _MI; }
    double z() const { return _z; }
    double w() const { return _w; }
    double R() const { return _R; }
    double imQ() const { return _im_q; }
    double reQ() const { return _re_q; }
    double imQ1() const { return _im_q1; }
    double reQ1() const { return _re_q1; }
    Lock lock() const { return _lock; }
    Zone zone() const { return _zone; }

    void setLambda(const double& value) { _lambda = value; _ref = Ref::Lambda; }
    void setW0(const double& value) { _w0 = value; _ref = Ref::W0; }
    void setZ0(const double& value) { _z0 = value; _ref = Ref::Z0; }
    void setVs(const double& value) { _Vs = value; _ref = Ref::Vs; }
    void setMI(const double& value) { _MI = value; _ref = Ref::MI; }
    void setZ(const double& value) { _z = value; _ref = Ref::Z; }
    void setW(const double& value) { _w = value; _ref = Ref::W; }
    void setR(const double& value) { _R = value; _ref = Ref::R; }
    void setReQ(const double& value) { _re_q = value; _ref = Ref::Q; }
    void setImQ(const double& value) { _im_q = value; _ref = Ref::Q; }
    void setReQ1(const double& value) { _re_q1 = value; _ref = Ref::Q1Re; }
    void setImQ1(const double& value) { _im_q1 = value; _ref = Ref::Q1Im; }
    void setLock(Lock value) { _lock = value; }
    void setZone(Zone value) { _zone = value; }
    void setRef(Ref value) { _ref = value; }

private:
    double _MI;               ///< Beam quality parameter.
    double _z0;               ///< Rayleight range.
    double _w0;               ///< Waist radius.
    double _Vs;               ///< Half of the divergence angle in the far field.
    double _z;                ///< Axial distance from the waist to wavefront.
    double _w;                ///< Beam radius at distance z from the waist.
    double _R;                ///< Wavefront ROC at distance z from the waist.
    double _re_q;             ///< Real part of the complex beam parameter at distance z.
    double _im_q;             ///< Imaginary part of the complex beam parameter at distance z.
    double _re_q1;            ///< Real part of the inverted complex beam parameter at distance z.
    double _im_q1;            ///< Imaginary part of the inverted complex beam parameter at distance z.
    double _lambda;           ///< Wavelength.
    Lock _lock = Lock::Waist; ///< Defines wich group of parameters should be fixed.
    Zone _zone = Zone::Far;   ///< Define wich solution of quadratic equation for z(R)
                              ///< should be taken. It is used in `Lock::Front` mode.
    Ref _ref = Ref::W0;       ///< Reference parameter, base for calculation of all others.

    void calc_Vs_from_W0_MI();
    void calc_W_from_W0_Z_Z0();
    void calc_R_from_Z_Z0();
    void calc_Q1_from_W_R();
    void calc_Q_from_Q1();
    void calc_Q1_from_Q();
    void calc_W_from_Q1();
    void calc_R_from_Q1();
    void calc_MI_from_W0_Z0();
    void calc_MI_from_W0_Vs();
    void calc_Z0_from_W0_MI();
    void calc_Z0_from_R_Z();
    void calc_Z0_from_W0_W_Z();
    void calc_W0_from_W_Z_Z0();
    void calc_W0_from_Z0_MI();
    void calc_Z_from_R_Z0();
    void calc_Z_from_R_W_W0();
    void calc_Z_from_W_R_Vs();
    void calc_Z_from_R_W_MI();
};

#endif // GAUSS_CALCULATOR_H
