#!/usr/bin/env python

import numpy as np
from math import *
from rezonator import *

n = 1.2     # IOR
L = 0.088   # length of plate
L1 = L * 0.2
L2 = L - L1

print('----------------------------------')
print_res('Optical path', L * n)
print_res('Subrange', L1)

print('----------------------------------')
print('Precalculated full matrix')
# ../help/matrix/ElemPlate.png
M_full = make_abcd(1, L / n, 0, 1)
print_abcd("M_full", M_full)

print('----------------------------------')
print('Product of 4 matrices')

# input into medium
M_in = make_abcd(1, 0, 0, 1/n)
# left half-pass throught medium
M_1 = make_abcd(1, L1, 0, 1)
# right half-pass throught medium
M_2 = make_abcd(1, L2, 0, 1)
# output into air
M_out = make_abcd(1, 0, 0, n)

# full matrix - production of prev 4 matrices
M_4 = np.linalg.multi_dot([M_out, M_2, M_1, M_in])
print_abcd("M_4(4)", M_4)
compare_abcd("M_full == M_4", M_full, M_4)

print('----------------------------------')
print('Product of 2 matrices')

# input into medium and left half-pass
# ../help/matrix/ElemPlate_left.png
M_lf = make_abcd(1, L1/n, 0, 1/n)
print_abcd("M_lf", M_lf) 

# right half-pass and output into air
# ../help/matrix/ElemPlate_right.png
M_rt = make_abcd(1, L2, 0, n)
print_abcd("M_rt", M_rt) 

# matrix of plate - production of prev 2 matrices
M_2 = np.dot(M_rt, M_lf)
print_abcd("M_2", M_2) 
compare_abcd("M_full == M_2", M_full, M_2)
