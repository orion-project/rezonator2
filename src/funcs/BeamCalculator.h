#ifndef BEAM_CALCULATOR_H
#define BEAM_CALCULATOR_H

#include <math.h>
#include <QVector>

/**
  Класс для пересчета одних параметров гауссова пучка в другие.

  Параметры пучка считаются на расстоянии Z от перетяжки.
  Фиксированное положение - может быть фронт или перетяжка, фактически
  нужно только затем, чтобы указать, какие параметры пересчитывать при
  изменении значения качества пучка (M^2). Если зафиксирован фронт, то
  при изменении M^2 находится новая перетяжка, которая соответствует размеру
  и кривизне пучка (тоже что комплексному параметру) на заданном расстоянии.
  Наоборот, если зафиксирована перетяжка, то при изменении M^2 находится
  новый размер и кривизна пучка (комплексный параметр) на заданном расстоянии
  при неизменной перетяжке. (Значения bcrQ, bcrQ1 используется внутренне, чтобы
  показать, что нужно рассчитать размер и кривизну из комплексного (или обратного)
  параметра, а не наоборот. По смыслу это все равно фиксированный фронт.)

  Класс не умеет вычислять M^2 на основании, скажем, размера перетяжки и размера
  пучка на заданном расстоянии. M^2 это всегда входной параметр.

  Для сохранения независимости, класс не использует системные или схемные
  единицы измерения. Все вычисления ведутся в системе СИ. Углы в радианах.
  Перевод параметров из используемых пользовательских единиц измерения в СИ
  и обратный перевод результатов из СИ в пользовательские единицы измерения
  должен обеспечивать интерфейс (окно), в котором используется объект класса.

  Алгоритм использования:
  1) Создать объект
  2) Задать M^2, перетяжку и расстояние
    или Задать M^2, размер пучка и кривизну фронта
    или Задать M^2 и комплексный параметр пучка
    или Задать M^2 и обратный комплексный параметр
  3) Вызвать метод Calc
  4) Можно использовать остальные параметры
*/
class BeamCalculator
{
public:
    enum class Lock {Waist, Front};
    enum class Zone {Near, Far};

    void calc();

    double lambda() const { return _lambda; }
    double w0() const { return _w0; }
    double z0() const { return _z0; }
    double Vs() const { return _Vs; }
    double M2() const { return _M2; }
    double z() const { return _z; }
    double w() const { return _w; }
    double R() const { return _R; }
    double imQ() const { return _im_q; }
    double reQ() const { return _re_q; }
    double imQ1() const { return _im_q1; }
    double reQ1() const { return _re_q1; }
    Lock lock() const { return _lock; }
    Zone zone() const { return _zone; }

    void setLambda(const double& value) { _lambda = value, _ref = Ref::Lambda; }
    void setW0(const double& value) { _w0 = value, _ref = Ref::W0; }
    void setZ0(const double& value) { _z0 = value, _ref = Ref::Z0; }
    void setVs(const double& value) { _Vs = value, _ref = Ref::Vs; }
    void setM2(const double& value) { _M2 = value, _ref = Ref::M2; }
    void setZ(const double& value) { _z = value, _ref = Ref::Z; }
    void setW(const double& value) { _w = value, _ref = Ref::W; }
    void setR(const double& value) { _R = value, _ref = Ref::R; }
    void setReQ(const double& value) { _re_q = value, _ref = Ref::Q; }

    // Мнимая часть КПП всегда должна быть отрицательной, это следует из
    // определения обратного КПП - его мнимая часть всегда положительна.
    // КПП находится как Conj(ОКПП)/Mod(ОКПП) - модуль положителен по определению,
    // а комплексно сопряженное от положительной мнимой части даст отрицательную
    // мнимую часть результата
    void setImQ(const double& value) { _im_q = -fabs(value), _ref = Ref::Q; }

    void setReQ1(const double& value) { _re_q1 = value, _ref = Ref::Q1Re; }
    void setImQ1(const double& value) { _im_q1 = value, _ref = Ref::Q1Im; }
    void setLock(Lock value) { _lock = value; }
    void setZone(Zone value) { _zone = value; }

private:
    enum class Ref {Lambda, W0, Z0, Vs, M2, Z, W, R, Q, Q1Im, Q1Re};

    double _M2 = 1.0;         // параметр качества пучка
    double _z0 = 0.0;         // радиус дифракционной рассходимости
    double _w0 = 100e-6;      // радиус перетяжки
    double _Vs = 0.0;         // половинный угол расходимости в дальней зоне
    double _z = 0.1;          // расстояние от перетяжки
    double _w = 0.0;          // радиус пучка на расстоянии Z
    double _R = 0.0;          // радиус кривизны волнового фронта на расстоянии Z
    double _re_q = 0.0;       // комплексный параметр пучка на расстоянии Z
    double _im_q = 0.0;       // мнимая часть КПП
    double _re_q1 = 0.0;      // обратный комплексный параметр пучка на расстоянии Z
    double _im_q1 = 0.0;      // мнимая часть ОКПП
    double _lambda = 0.98e-6; // длина волны
    Lock _lock = Lock::Waist; // вычислять фронт или перетяжку при изменении параметров
    Zone _zone = Zone::Far;   // определяет какое решение брать при вычислении z из R,
                              // для ближней или дальней зоны. Применяется при Lock::Front
    Ref _ref = Ref::W0;       // исходя из этого параметра находятся все остальные

    void calc_Vs_from_W0_M2();
    void calc_W_from_W0_Z_Z0();
    void calc_R_from_Z_Z0();
    void calc_Q1_from_W_R();
    void calc_Q_from_Q1();
    void calc_Q1_from_Q();
    void calc_W_from_Q1();
    void calc_R_from_Q1();
    void calc_M2_from_W0_Z0();
    void calc_M2_from_W0_Vs();
    void calc_Z0_from_W0_M2();
    void calc_Z0_from_R_Z();
    void calc_Z0_from_W0_W_Z();
    void calc_W0_from_W_Z_Z0();
    void calc_W0_from_Z0_M2();
    void calc_Z_from_R_Z0();
    void calc_Z_from_R_W_W0();
    void calc_Z_from_W_R_Vs();
};

#endif // BEAM_CALCULATOR_H







