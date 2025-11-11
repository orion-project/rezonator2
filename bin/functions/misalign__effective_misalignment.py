'''
Effective misalignment

Calculate the effective misalignment of the whole system
and the position of the effective optical axis of the resonator
using extended ray matrices of size 33 (ABCDEF method).
'''
from math import degrees as deg
import rezonator as Z
from rezonator import Matrix3
import schema

def calculate():
  rt = schema.round_trip(ref='M_out')

  # Matrix3 is a helper class for extended ray matrix.
  # It can be constructed from a conventional ABCD matrix
  # and additional components assigned manually.
  M = Matrix3()
  for i in range(rt.matrix_count):
    m = Matrix3(rt.matrix(i))
    elem = rt.elem(i)
    # Additional matrix components are assigned from
    # particular element's misalignments.
    # Element's method `param` returns a parameter value in SI units.
    # The second argument is a default value in SI units
    # used when the parameter is not found.
    # This variant is convenient for using with custom parameters
    # that can be added optionally to only some desired elements.
    m.E = elem.param('dYt', 0)
    m.F = elem.param('dVt', 0)
    # Multiply ABCDEF matrices in the same order
    # as the initial ABCD matrices are stored
    # in the automatically prepared round-trip
    M *= m

  Z.print('Effective misalignment at', rt.ref.label)
  E = M.E
  F = M.F
  Z.print(E*1000, 'mm', deg(F), '°')

  Z.print('Position of effective optical axis at', rt.ref.label)
  A, B, C, D = rt.matrix()
  E0 = ((1 - D)*E + B*F) / (2 - A - D)
  F0 = (C*E + (1 - A)*F) / (2 - A - D)
  Z.print(E0*1000, 'mm', deg(F0), '°')
