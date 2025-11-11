'''
Misalignments at elements
'''
import rezonator as Z
from rezonator import Matrix3
import schema

def columns():
  # This function is automatically called by the table window
  # to get descriptions of columns that will be displayed.
  return [
    # Effective misalignment
    {'label': 'Y', 'title': 'Displacement', 'dim': Z.DIM_LINEAR },
    {'label': 'V', 'title': 'Tilt', 'dim': Z.DIM_ANGULAR },
    # Position of effective optical axis
    {'label': 'Y0', 'title': 'Axis displacement', 'dim': Z.DIM_LINEAR },
    {'label': 'V0', 'title': 'Axis tilt', 'dim': Z.DIM_ANGULAR },
  ]

def calculate(elem, pos, rt):
  M = Matrix3()
  for i in range(rt.matrix_count):
    m = Matrix3(rt.matrix(i))
    elem = rt.elem(i)
    if rt.plane == Z.PLANE_T:
      m.E = elem.param('dYt', 0)
      m.F = elem.param('dVt', 0)
    else:
      m.E = elem.param('dYs', 0)
      m.F = elem.param('dVs', 0)
    M *= m
  return {
    'Y': M.E,
    'V': M.F,
    'Y0': ((1 - M.D)*M.E + M.B*M.F) / (2 - M.A - M.D),
    'V0': (M.C*M.E + (1 - M.A)*M.F) / (2 - M.A - M.D),
  }

