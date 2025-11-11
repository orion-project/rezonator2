'''
Effective misalignment (alt)

Calculation the effective misalignment of the whole system
and the position of the effective optical axis of the resonator.
The analytical solution for multiplication of ABCD matrices
by element misalignment vectors
is used instead of extended ABCDEF ray matrices.
In particular, this solution is shown in the book
"A. Gerrard and J. M. Burch,
Introduction to Matrix Methods in Optics", Appendix II.
However it's more straightforward just to construct
ABCDEF matrices and multiply them, so there is no big reason
to directly use this solution. We just show it for generality
and to have additional verification for algorithms.
'''
from math import degrees as deg
import rezonator as Z
from rezonator import Matrix, RayVector
import schema

def calculate():
  rt = schema.round_trip(ref='M_out')
  M = rt.matrix()

  # RayVector is an additional reZonator's helper class
  # containing two components and having an ability
  # to be multiplyed by ray matrices.
  YV = RayVector()
  for i in range(rt.matrix_count):
    # Temp matrix for calculation of misalighnments
    # after i-th element in the round-trip.
    m = Matrix()
    for j in range(i):
      m *= rt.matrix(j)
    elem = rt.elem(i)
    YV += m * RayVector(
      # Element's method `param` returns a parameter value in SI units.
      # The second argument is a default value in SI units
      # used when the parameter is not found.
      # This variant is convenient for using with custom parameters
      # that can be added optionally to only some desired elements.
      elem.param('dYt', 0),
      elem.param('dVt', 0))

  Z.print('Effective misalignment at', rt.ref.label)
  Y, V = YV
  Z.print(Y*1000, 'mm', deg(V), '°')

  Z.print('Position of effective optical axis at', rt.ref.label)
  A, B, C, D = M
  Y0 = ((1 - D)*Y + B*V) / (2 - A - D)
  V0 = (C*Y + (1 - A)*V) / (2 - A - D)
  Z.print(Y0*1000, 'mm', deg(V0), '°')

