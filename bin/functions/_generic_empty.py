'''
Custom code
'''
import rezonator as Z
import schema

def calculate():
  Z.print('reZonator', Z.version())

  if schema.elem_count() == 0:
    raise Exception('Schema is empty')

  # Code your logic here