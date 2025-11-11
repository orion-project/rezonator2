'''File IO demos'''

import rezonator as Z
import configparser
import csv
import json
import os
import tempfile

def calculate():
  Z.print('**** Current dirtectory')
  Z.print(os.getcwd())

  Z.print('\n**** Read-write text file')
  with open('examples/telescope.she', 'rt') as f:
    with tempfile.NamedTemporaryFile('wt') as tmp:
      for line in f:
        tmp.write(line)
      Z.print('Temp file written:', tmp.name)

  Z.print('\n**** Read INI file')
  old_schema = configparser.ConfigParser()
  old_schema.read('examples/telescope.she')
  Z.print('Version:', old_schema['PREFERENCES']['Version'])
  Z.print('Title:', old_schema['PREFERENCES']['Title'])

  Z.print('\n**** Parse JSON string')
  json_str = json.dumps({'a': 'A', 'b': 2})
  Z.print(json_str)
  json_obj = json.loads(json_str)
  Z.print('a=', json_obj['a'], ', b=', json_obj['b'], spaced=False)

  Z.print('\n**** Read JSON file')
  with open(os.path.join('examples', 'singlet_thick.rez')) as f:
    schema = json.load(f)
    Z.print('Version:', schema['schema_version'])
    Z.print('Title:', schema['title'])

  Z.print('\n**** Read CSV file')
  with open('examples/ior.csv', 'r') as f:
    reader = csv.DictReader(f)
    for row in reader:
      Z.print(row['Material'], row['IOR'])

