'''
Caustic (py)

 This is a function name that is displayed in function window headers.
In the plot title, it is substituted instead of the {func_name} variable.
'''
import rezonator as Z
import schema

# Find an element that has a length; it will be a function argument.
# Do this step in the global scope before any function is called
# to be able to use the element in the X-axis title (see figure() below).

elem = None
for i in range(schema.elem_count()):
  # Unlike standard Python arrays, elements are indexed from 1
  # for consistency with how they are numbered in the elements table.
  el = schema.elem(i+1)
  if el.length is not None:
    elem = el
    break;
if not elem:
  raise Exception('No suitable lement')

def figure():
  '''
  This function is automatically called by the function window
  to configure the plot properties.
  '''
  return {
    # Value dimension used when choosing a measurement unit for the axis X.
    'x_dim': Z.DIM_LINEAR,
    # Default title for X-axis
    # which is substituted instead of the {default_title} variable.
    'x_title': f'{elem.label if elem else None}',

    # Value dimension used when choosing a measurement unit for the axis Y.
    'y_dim': Z.DIM_LINEAR,
    # Default title for Y-axis
    # which is substituted instead of the {default_title} variable.
    'y_title': 'Beam radius',
  }

def calculate():
  Z.print('Calc caustic at', elem.label)
  (xt, yt) = calc_plane('t', 100)
  (xs, ys) = calc_plane('s', 100)
  return [
  # Return a list of lines.
  # Labels can be arbitrary; they are displayed in the legend.
  # There can be several lines with the same label;
  # they are considered as parts of a single multi-segment graph.
  # 'T' and 'S' are standard labels for tangential and sagittal planes.
  # For such lines, the standard graph formats are used,
  # though they can be overridden with a user format.
    { 'label': 'T', 'x': xt, 'y': yt },
    { 'label': 'S', 'x': xs, 'y': ys }
  ]

def calc_plane(plane, point_count):
  '''
  Calculate beam radius inside a given element
  for a given work plane - T or Z.
  '''
  rt = schema.round_trip(ref=elem, inside=True, plane=plane)
  step = elem.axial_length/(point_count-1)
  x, y = [], []
  for i in range(point_count):
    offset = step*i
    elem.offset = offset
    x.append(offset)
    y.append(rt.beam_radius())
  return (x, y)

