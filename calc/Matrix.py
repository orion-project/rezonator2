#!/usr/bin/env python

import numpy as np
from rezonator import *

print('----------------------------------')
print('Matrix * matrix:')
m1 = [[5, 6], [7, 8]]
m2 = [[1, 2], [3, 4]]
m3 = np.matmul(m1, m2)
print(m3)
m4 = np.matmul(m2, m1)
print(m4)

print('----------------------------------')
print('Transformation of complex ROC by ray matrix:')
m1 = np.matrix([[5, 6], [7, 8]])
q1 = 1 + 2j
q2 = transform_q(q1, m1)
print(q2)
