'''
Custom table

↑ This is a function name that is displayed in function window headers.
'''
import rezonator as Z
import schema

def columns():
  '''
  This function is automatically called by the table window
  to get descriptions of columns that will be displayed.
  - label
    a short name of the calculated value
    it is displayed in the column title and also used for extracting values
    from a result dict returned by the calculate function
  - title
    a longer human-readable name of the calculated value
    it is displayed in menus of the table window
  - dim
    a value dimension used when choosing a measurement unit for the column
  '''
  return [
    {
      'label': 'W',
      'title': 'Beam radius',
      'dim': Z.DIM_LINEAR
    },
    {
      'label': 'R',
      'title': 'Wavefront ROC',
      'dim': Z.DIM_LINEAR
    },
    {
      'label': 'V',
      'title': 'Half div. angle',
      'dim': Z.DIM_ANGULAR
    },
  ]

def calculate(elem, pos, rt):
  '''
  This function is automatically called for each element,
  or several times for the same element with different 'pos'
  depending on the options selected in the table window.
  In such a simple form without any additional work,
  the custom function will do the same as what
  the built-in function 'Beam Parameters at Elements' does.
  '''
  (w, r, v) = rt.beam()
  return {
    'W': w,
    'R': r,
    'V': v
  }
