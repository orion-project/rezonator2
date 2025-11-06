'''
Custom code

↑ This is a human readable name that is displayed in the code window header.
'''
import rezonator as Z
import schema

def calculate():
  '''
  This function is automatically called when the Run button is clicked.
  '''
  Z.print('reZonator', Z.version())

  if schema.elem_count() == 0:
    raise Exception('Schema is empty')

  # Code your logic here