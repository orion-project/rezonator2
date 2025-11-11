'''
Misaligned axis position

 This is a function name that is displayed in function window headers.
In the plot title, it is substituted instead of the {func_name} variable.
'''
import rezonator as Z
from rezonator import Matrix3
import schema

# Specify which elements will be the function arguments.
# Do this step in the global scope before any function is called
# to be able to use elements in axes titles (see figure() below).
elem_var = schema.elem('M_foc')
elem_at = schema.elem('M_out')

def figure():
  '''
  This function is automatically called by the function window
  to configure the plot properties.
  '''
  return {
    # A value dimension used when choosing a measurement unit for the axis X.
    'x_dim': Z.DIM_LINEAR,
    # Default title for X-axis
    # which is substituted instead of the {default_title} variable.
    'x_title': f'ROC of {elem_var.label}',
    # A value dimension used when choosing a measurement unit for the axis Y.
    'y_dim': Z.DIM_LINEAR,
    # Default title for Y-axis
    # which is substituted instead of the {default_title} variable.
    'y_title': f'Axis displacement at {elem_at.label}',
  }

def calculate():
  Z.print('Calc axis displacement at', elem_at.label,
    'when variating', elem_var.label, 'ROC')

  # Plot parameters
  start = 0.040
  stop = 0.120
  point_count = 1000
  step = (stop - start)/(point_count-1)

  rt = schema.round_trip(ref=elem_at)
  xx = []
  yy = []
  # Lock the element whose parameter is changing during plotting.
  # This allows us to preserve its original parameter value
  # and to automatically restore it after plotting.
  # Also, it disables UI updating and recalculation of built-in functions
  # when element parameters change.
  elem_var.lock()
  try:
    for i in range(point_count):
      # Gradually change the varying element parameter
      # and calculate the graph Y value at each step
      x = start + step*i
      elem_var.set_param('R', x)

      # Get the round-trip matrix
      # after an element's parameter has been assigned a new value
      # (and therefore the element's matrix has been changed)
      # to force the round-trip to recompute the matrix production.
      A, B, C, D = rt.matrix()

      # Skip points where resonator is not stable
      if rt.stabil_nor <= -1 or rt.stabil_nor >= 1:
        xx.append(x)
        yy.append(float('nan'))
        continue

      tmp = Matrix3()
      for i in range(rt.matrix_count):
        m = Matrix3(rt.matrix(i))
        m.E = rt.elem(i).param('dEt', 0)
        m.F = rt.elem(i).param('dVt', 0)
        tmp *= m
      E = tmp.E
      F = tmp.F

      E0 = ((1 - D)*E + B*F) / (2 - A - D)
      #F0 = (C*E + (1 - A)*F) / (2 - A - D)
      xx.append(x)
      yy.append(E0)
  finally:
    elem_var.unlock()
  return [
  # Return a list of lines.
  # Labels can be arbitrary; they are displayed in the legend.
  # There can be several lines with the same label;
  # they are considered as parts of a single multi-segment graph.
    {
     'label': 'Axis offset',
     'x': xx,
     'y': yy
    },
  ]

