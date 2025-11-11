'''
Beam data (code)
'''
import rezonator as Z
import schema

def columns():
  return [
    # Return the same columns as the built-in
    # function 'Beam Parameters at Elements' has
    { 'label': 'W', 'title': 'W', 'dim': Z.DIM_LINEAR },
    { 'label': 'R', 'title': 'R', 'dim': Z.DIM_LINEAR },
    { 'label': 'V', 'title': 'V', 'dim': Z.DIM_ANGULAR },
    # Plus additional columns for the same values
    # but calculated a bit differently (see below)
    { 'label': 'W1', 'title': 'W1', 'dim': Z.DIM_LINEAR },
    { 'label': 'R1', 'title': 'R1', 'dim': Z.DIM_LINEAR },
    { 'label': 'V1', 'title': 'V1', 'dim': Z.DIM_ANGULAR },
  ]

def calculate(elem, pos, rt):
  # This function is automatically called for each element,
  # or several times for the same element with different 'pos'
  # depending on the options selected in the table window.
  # Round-trip is also prepared by the table winwdow
  # with correct reference element for each position.
  # In such a simple form without any additional work,
  # the custom function will do the same as what
  # the built-in function 'Beam Parameters at Elements' does.
  (w, r, v) = rt.beam()

  # Additionally, let's calculate the same values but building
  # a round-trip right here in place. This allows for
  # additional verification for the target element and
  # result position are passed properly.
  # POS_LEFT, POS_BEG, POS_MID, POS_END, POS_RIGHT -
  # these constants are automatically visible in the code.
  if pos == POS_LEFT:
    # Calculate rigth before the element.
    # In this case a round-trip reference should be the previous element
    rt1 = schema.round_trip(ref=elem.prev, plane=rt.plane)
    (w1, r1, v1) = rt1.beam()
  elif pos == POS_BEG:
    # The target element has a length
    # and we calculate at the very start and inside of it.
    # Here we don't care if the element has IOR or not
    # this will be automatically respected by the 'round_trip' method
    rt1 = schema.round_trip(ref=elem, plane=rt.plane, inside=True)
    elem.offset = 0
    (w1, r1, v1) = rt1.beam()
  elif pos == POS_MID:
    # The target element has a length
    # and we calculate in the middle and inside of it.
    rt1 = schema.round_trip(ref=elem, plane=rt.plane, inside=True)
    elem.offset = elem.axial_length/2
    (w1, r1, v1) = rt1.beam()
  elif pos == POS_END:
    # The target element has a length
    # and we calculate at the very end and inside of it.
    rt1 = schema.round_trip(ref=elem, plane=rt.plane, inside=True)
    elem.offset = elem.axial_length
    (w1, r1, v1) = rt1.beam()
  else:
    # Calculate right after the element taking it as a whole.
    rt1 = schema.round_trip(ref=elem, plane=rt.plane)
    (w1, r1, v1) = rt1.beam()

  # Returned dict should have keys matching the labels
  # returned by the 'columns' function
  return {
    'W': w,
    'R': r,
    'V': v,
    'W1': w1,
    'R1': r1,
    'V1': v1,
  }

