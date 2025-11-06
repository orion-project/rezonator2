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
  pass