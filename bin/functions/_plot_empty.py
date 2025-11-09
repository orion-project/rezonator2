'''
Custom plot

↑ This is a function name that is displayed in function window headers
and in the plot title.
'''
import rezonator as Z
import schema

def figure():
  '''
  This function is automatically called by the function window
  to configure the plot properties.
  '''
  return {
    # A value dimension used when choosing a measurement unit for the axis X.
    'x_dim': Z.DIM_LINEAR,
    # A value dimension used when choosing a measurement unit for the axis Y.
    'y_dim': Z.DIM_LINEAR,
  }

def calculate():
  return [
  # Return a list of lines.
  # Labels can be arbitrary, they are displayed in legend.
  # There can be several lines with the same label,
  # they are considered as parts of a single multi-segment graph.
    {
     'label': 'T',
     'x': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
     'y': [4, 3, 2, 1, 0, 0, 1, 2, 3, 4]
    },
    {
     'label': 'S',
     'x': [0.0, 1.0, 2.0, 3.0, 4.0,  5.0, 6.0, 7.0, 8.0, 9.0],
     'y': [4.5, 3.4, 2.3, 1.2, 0.1, -0.1, 1.1, 2.2, 3.3, 4.4]
    }
  ]