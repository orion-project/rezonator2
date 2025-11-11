'''
Round-trip matrix

A custom implementation of the standard builtin function "Round-Trip Matrix"
'''

import rezonator as Z
import schema

def calculate():
  ref_index = 4
  ref_elem = schema.elem(ref_index)
  Z.print('Ref:', ref_elem.label)

  if ref_elem.length is not None:
    ref_elem.offset = ref_elem.length / 2.0
    Z.print('Offset inside:', ref_elem.offset * 1000, 'mm')

  rt_t = schema.round_trip(ref=ref_index, split_range=True, plane=Z.PLANE_T)
  rt_s = schema.round_trip(ref=ref_index, split_range=True, plane=Z.PLANE_S)
  Z.print('Mt:', rt_t.matrix)
  Z.print('Ms:', rt_s.matrix)
  Z.print('Pt (normal):', rt_t.stabil_nor)
  Z.print('Ps (normal):', rt_s.stabil_nor)
  Z.print('Pt (squared):', rt_t.stabil_sqr)
  Z.print('Ps (squared):', rt_s.stabil_sqr)

