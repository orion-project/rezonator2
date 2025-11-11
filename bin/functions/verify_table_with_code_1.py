'''
Beam data (py code)

Calculates the same beam data as the 'Beam Parameters at Elements'
function does but using custom Python code.
This allows for cross-verification of built-in functions
and the Python interface for user scripts.
'''
import rezonator as Z
import schema

POS_LENS_LF   = '->()'
POS_LENS_RT   = '  ()->'
POS_CRYST_IN  = '->[   ]'
POS_RANGE_BEG = '  [-> ]'
POS_RANGE_MID = '  [ + ]'
POS_RANGE_END = '  [ ->]'
POS_CRYST_OUT = '  [   ]->'
POS_IFACE_LF  = '->|'
POS_IFACE_RT  = '  |->'

def calculate():
  plane = Z.PLANE_T
  result = {
    '': ('W', 'R', 'V')
  }

  # Calculates beam parameters from the current round-trip
  def calc(pos='', offset=None):
    if offset is not None:
      elem.offset = offset
    p = (' ' + pos) if pos else ''
    # result[elem.label + p] = (
    #   Z.format(rt.beam_radius()*1e6),
    #   Z.format(rt.front_radius()*100),
    #   Z.format(rt.half_angle()*1000)
    # )
    # This is an optimized variant to calc beam params in one step.
    # It's a bit faster because the round-trip matrix multiplication
    # happens only once and then all props are calculated.
    (w, r, v) = rt.beam()
    result[elem.label + p] = (
      Z.format(w*1e6),
      Z.format(r*100),
      Z.format(v*1000)
    )

  elem = schema.elem('M1')
  rt = schema.round_trip(ref=elem, plane=plane)
  calc()

  elem = schema.elem('d1')
  rt = schema.round_trip(ref=elem, plane=plane, inside=True)
  # For empty space, it doesn't matter
  # do we calculate inside the space at its very start with `inside=True`,
  # or before it, taking the previous element as a round-trip reference,
  # because an empty space has IOR = 1.
  # Here '->[   ]' === '  [-> ]' (POS_CRYST_IN === POS_RANGE_BEG)
  calc(pos=POS_RANGE_BEG, offset=0)
  calc(pos=POS_RANGE_MID, offset=elem.length/2)
  # For empty space, it doesn't matter
  # do we calculate inside the space at its very end with `inside=True`,
  # or after it, accounting the whole element with `inside=False`,
  # because an empty space has IOR = 1.
  # Here '  [ ->]' === '  [   ]->' (POS_RANGE_END === POS_CRYST_OUT)
  calc(pos=POS_RANGE_END, offset=elem.length)

  elem = schema.elem('s1')
  # To calc before an interface, a ref must be the previous element.
  # An IOR of the left neighbour element will be used.
  #
  # NOTE:
  #   For this particulat schema and element s1
  #   it's fine to get the previous element as `ref=elem.index-1`.
  #   But here it's done just to give an example.
  #   We simply get the previous index from schema's
  #   internal element array, which has some issues:
  #   - element disabling is not respected
  #   - round-trip rules are not respected
  #   For generality, element's properties `prev` and `next`
  #   should be used instead: `ref=elem.prev` (see below)
  #
  rt = schema.round_trip(ref=elem.index-1, plane=plane)
  calc(pos=POS_IFACE_LF)
  # After the interface, an IOR of the roght neighbour element
  # will be used for beamsize calculation.
  rt = schema.round_trip(ref=elem, plane=plane)
  calc(pos=POS_IFACE_RT)

  elem = schema.elem('d2')
  # See comments for d1
  rt = schema.round_trip(ref=elem, plane=plane, inside=True)
  calc(pos=POS_RANGE_BEG, offset=0)
  calc(pos=POS_RANGE_MID, offset=elem.length/2)
  calc(pos=POS_RANGE_END, offset=elem.length)

  elem = schema.elem('G1')
  # To calc before a plate, a ref must be the previous element
  rt = schema.round_trip(ref=elem.prev, plane=plane)
  calc(pos=POS_CRYST_IN)
  # Unlike empty spaces, in crystals
  # beam parameters ouside the element ('->[   ]') is different from
  # those at the element's very start ('[-> ]') with `inside=True`.
  rt = schema.round_trip(ref=elem, plane=plane, inside=True)
  calc(pos=POS_RANGE_BEG, offset=0)
  # Unlike crystal (or rod), in a tilted plate,
  # a value of the Length parameter is different
  # from the beam's path it travels inside the plate.
  # The axial_length property accounts the plate's angle
  # and returns a geometrical distance between the beam's
  # input and output points at the plate's edges.
  calc(pos=POS_RANGE_MID, offset=elem.axial_length/2)
  calc(pos=POS_RANGE_END, offset=elem.axial_length)
  rt = schema.round_trip(ref=elem, plane=plane)
  calc(pos=POS_CRYST_OUT)

  elem = schema.elem('d3')
  rt = schema.round_trip(ref=elem, plane=plane, inside=True)
  calc(pos=POS_RANGE_BEG, offset=0)
  calc(pos=POS_RANGE_MID, offset=elem.length/2)
  calc(pos=POS_RANGE_END, offset=elem.length)

  elem = schema.elem('M2')
  # To calc before a mirror or lens
  # or any element that changes the wavefront,
  # a ref must be the previous element.
  rt = schema.round_trip(ref=elem.index-1, plane=plane)
  calc(pos=POS_LENS_LF)
  rt = schema.round_trip(ref=elem, plane=plane)
  calc(pos=POS_LENS_RT)

  elem = schema.elem('d4')
  rt = schema.round_trip(ref=elem, plane=plane, inside=True)
  calc(pos=POS_RANGE_BEG, offset=0)
  calc(pos=POS_RANGE_MID, offset=elem.length/2)
  calc(pos=POS_RANGE_END, offset=elem.length)

  elem = schema.elem('s2')
  rt = schema.round_trip(ref=elem.prev, plane=plane)
  calc(pos=POS_IFACE_LF)
  rt = schema.round_trip(ref=elem, plane=plane)
  calc(pos=POS_IFACE_RT)

  elem = schema.elem('TM1')
  rt = schema.round_trip(ref=elem, plane=plane, inside=True)
  calc(pos=POS_RANGE_BEG, offset=0)
  calc(pos=POS_RANGE_MID, offset=elem.length/2)
  calc(pos=POS_RANGE_END, offset=elem.length)

  elem = schema.elem('M3')
  rt = schema.round_trip(ref=elem.index-1, plane=plane)
  calc(pos=POS_LENS_LF)
  rt = schema.round_trip(ref=elem, plane=plane)
  calc(pos=POS_LENS_RT)

  elem = schema.elem('GM1')
  rt = schema.round_trip(ref=elem, plane=plane, inside=True)
  calc(pos=POS_RANGE_BEG, offset=0)
  calc(pos=POS_RANGE_MID, offset=elem.length/2)
  calc(pos=POS_RANGE_END, offset=elem.length)

  elem = schema.elem('s3')
  rt = schema.round_trip(ref=elem.prev, plane=plane)
  calc(pos=POS_IFACE_LF)
  rt = schema.round_trip(ref=elem, plane=plane)
  calc(pos=POS_IFACE_RT)

  elem = schema.elem('d5')
  rt = schema.round_trip(ref=elem, plane=plane, inside=True)
  calc(pos=POS_RANGE_BEG, offset=0)
  calc(pos=POS_RANGE_MID, offset=elem.length/2)
  calc(pos=POS_RANGE_END, offset=elem.length)

  elem = schema.elem('GA1')
  rt = schema.round_trip(ref=elem.prev, plane=plane)
  calc(pos=POS_LENS_LF)
  rt = schema.round_trip(ref=elem, plane=plane)
  calc(pos=POS_LENS_RT)

  elem = schema.elem('d6')
  rt = schema.round_trip(ref=elem, plane=plane, inside=True)
  calc(pos=POS_RANGE_BEG, offset=0)
  calc(pos=POS_RANGE_MID, offset=elem.length/2)
  calc(pos=POS_RANGE_END, offset=elem.length)

  elem = schema.elem('M4')
  rt = schema.round_trip(ref=elem, plane=plane)
  calc()

  # Show results in a table-like form
  width_p = 0
  width_w = 0
  width_r = 0
  width_v = 0
  for p in result:
    (w, r, v) = result[p]
    width_p = max(width_p, len(p))
    width_w = max(width_w, len(w))
    width_r = max(width_r, len(r))
    width_v = max(width_v, len(v))
  for p in result:
    (w, r, v) = result[p]
    Z.print( \
      f'{p:<{width_p}}', '|',
      f'{w:<{width_w}}', '|',
      f'{r:<{width_r}}', '|',
      f'{v:<{width_v}}', '|',
    )

