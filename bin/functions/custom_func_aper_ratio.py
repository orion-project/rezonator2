'''
Aperture ratio
'''
import rezonator as Z
import schema

def columns():
  '''
  Return table columns that will be displayed in the function window.
  '''
  return [
    {
      'label': 'W',
      'title': 'Beam radius',
      'dim': Z.DIM_LINEAR
    },
    {
      'label': 'AR',
      'title': 'Aperture ratio',
      'dim': Z.DIM_NONE
    },
    {
      'label': 'AR2',
      'title': 'Square ratio',
      'dim': Z.DIM_NONE
    },
  ]

def calculate(elem, pos, rt):
  # This function is automatically called for each element,
  # or several times for the same element with different 'pos'
  # depending on the options selected in the table window.

  def calc_ratio(aper):
    w = rt.beam_radius()
    # Here we treat the aperture size as radius
    # to be consistent with what is calculated for beam is radius.
    # But this is totally up to custom logic.
    ar = aper / w
    ar2 = ar**2
    return { 'W': w, 'AR': ar, 'AR2': ar2 }

  # We are only interested in elements having an aperture parameter.
  # It can be any parameter into which we include the meaning of an 'aperture'.
  # Here it's custom parameter created from built-in preset named A (aperture).
  aper = elem.param('A')
  if aper is not None:
    # POS_RIGHT means we calculate at the right outside of the element,
    # which also means AFTER or AT the element, considering it as one piece,
    # and its matrix included into the round-trip.
    if pos == POS_RIGHT:
      return calc_ratio(aper)

  # For crystal here we take two different apertures - on its left and right faces.
  # Not so it has a big sense in reality; it just demonstrates usage of custom
  # parameters and so the 'aperture' can be any of them that we call such.
  if elem.length is not None:
    if pos == POS_LEFT:
      aper = elem.param('A1')
      if aper is not None:
        return calc_ratio(aper)
    if pos == POS_RIGHT:
      aper = elem.param('A2')
      if aper is not None:
        return calc_ratio(aper)

  return None
