'''reZonator demos'''

import rezonator as Z
import schema

def calculate():
  Z.print('**** Global constants')
  Z.print(f'PI = {Z.PI}, C = {Z.C}m/s')

  Z.print('\n**** Schema parameteres')
  wl = schema.wavelength()
  p1 = schema.param("P1")
  Z.print(f'Wavelen = {wl*1e9}nm, P1 = {p1}')

  Z.print('\n**** Schema element')
  e = show_elem(schema.elem('Cr')) # can get element by label
  e = show_elem(schema.elem(2)) # can get element by number

def show_elem(e):
  Z.print('Element ', e.label, ':', spaced=False)
  Z.print('  length:', e.length, 'm')
  Z.print('  optical:', e.optical_path, 'm')
  Z.print('  axial:', e.axial_length, 'm')
  Z.print('  ior:', e.ior)

