'''
Repetition rate

A custom implementation of standard builtin function Repetition rate
'''

import rezonator as Z
import schema

def calculate():
  total_len = 0

  # schema returns elements as they are numbered in the table (1-based)
  # but python's range() function returns 0-based indexes, so +1 here
  for i in range(1, schema.elem_count()+1):
    elem = schema.elem(i)
    elem_len = elem.optical_path
    if not elem_len:
      # Is not a range
      continue
    Z.print('Elem', i, ':', elem.label, '=', Z.format(elem_len), 'm')
    total_len += elem_len

  Z.print('Total length: ', Z.format(total_len), 'm', spaced=False)

  # Effective length is doubled in standing-wave rezonatos
  if schema.is_sw():
    total_len *= 2

  rep_rate = Z.C / total_len
  Z.print('Repetition rate: ', Z.format(rep_rate/1e6), 'MHz', spaced=False)

