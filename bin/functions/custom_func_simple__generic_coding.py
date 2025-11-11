'''Generic coding'''

import rezonator as Z
import copy
import math
import platform
import pickle
import random
import statistics
import sys
import uuid
from decimal import Decimal
from typing import Dict

def calculate():
  Z.print('**** System info')
  Z.print('Python', platform.python_version(),
    'on', platform.system(), platform.version())
  for p in sys.path:
    Z.print('Module path:', p)

  x = [1, 2, 3, 4, 5, 6, 7, 8, 9]
  y = [1, 2, 3, 1, 2, 3, 1, 2, 3]
  Z.print('\n**** Math')
  Z.print('pi/2:', math.pi/2)
  Z.print('sin:', math.sin(math.pi/2))
  Z.print('cos:', math.cos(math.pi/2))
  Z.print('mean', statistics.mean(x))
  Z.print('median', statistics.median(x))
  Z.print('covariance', statistics.covariance(x, y))
  Z.print('correlation', statistics.correlation(x, y))
  Z.print('random', random.randint(1, 10))
  Z.print('random', random.random())
  Z.print('random', random.uniform(5, 10))
  Z.print('random', random.choice(x))
  Z.print('decimal', str(Decimal('1.33') + Decimal('1.27')))
  Z.print('decimal', str(Decimal(3).sqrt()))

  Z.print('\n**** Classes')
  m1 = Message('Hello world!')
  m1.print()
  m2 = Message(42)
  m2.print()
  m3 = copy.copy(m1)
  m3.print()
  m4 = copy.deepcopy(m2)
  m4.print()

  Z.print('\n**** Functions')
  print_typed_dict({'a': 'A', 'b': 2})

  Z.print('\n**** Pickle')
  pm1 = pickle.dumps(m1)
  pm2 = pickle.dumps(m2)
  Z.print('Pickled:', f'{pm1}', f'{pm2}')
  upm1 = pickle.loads(pm1)
  upm2 = pickle.loads(pm2)
  Z.print('Unpickled:', upm1.msg, upm2.msg)

  Z.print('\n**** UUID')
  uuid1 = uuid.uuid1()
  uuid3 = uuid.uuid3(uuid.NAMESPACE_DNS, 'orion-project.org')
  uuid4 = uuid.uuid4()
  uuid5 = uuid.uuid5(uuid.NAMESPACE_DNS, 'python.org')
  Z.print(f'{uuid1}')
  Z.print(f'{uuid3}')
  Z.print(f'{uuid4}')
  Z.print(f'{uuid5}')
  id1 = uuid.UUID('{00010203-0405-0607-0809-0a0b0c0d0e0f}')
  id2 = uuid.UUID(bytes=id1.bytes)
  Z.print(str(id1), str(id2))

# Class with typing
# (Python itself doesn't check for types)
class Message:
  def __init__(self, msg: str) -> None:
    self.msg = msg

  def print(self):
    Z.print(self.msg)


# Function with typing
# (Python itself doesn't check for types)
def print_typed_dict(d: Dict[str, str]) -> None:
  for k in d:
    Z.print(k, '=', d[k])

