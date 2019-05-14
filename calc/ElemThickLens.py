#!/usr/bin/env python

import numpy as np
from math import *
from rezonator import *

n = 1.5                # IOR
L = 0.01               # Thickness of lens
L1 = L * 0.2
L2 = L - L1
R1 = -0.09             # -->(//  R < 0
R2 = 0.15              # //)-->  R > 0

print('----------------------------------')
print_res('Optical path', L * n)
print_res('Subrange', L1)

print('----------------------------------')
print('Precalculated full matrix')
# ../help/matrix/ElemThickLens.png
A = 1 + L/R1*(n-1)/n
B = L/n
C = (n-1)*(1/R1 - 1/R2) - L/R1/R2*(n-1)*(n-1)/n
D = 1 - L/R2*(n-1)/n
M_full = make_abcd(A, B, C, D)
print_abcd("M(pre)", M_full)

R1_inv = -R2
R2_inv = -R1
A = 1 + L/R1_inv*(n-1)/n
B = L/n
C = (n-1)*(1/R1_inv - 1/R2_inv) - L/R1_inv/R2_inv*(n-1)*(n-1)/n
D = 1 - L/R2_inv*(n-1)/n
M_full_inv = make_abcd(A, B, C, D)
print_abcd("M(pre, inv)", M_full_inv)

print('----------------------------------')
print('Product of 4 matrices')

# input into medium
M_in = make_abcd(1, 0, (n-1)/R1/n, 1/n)
# left half-pass throught medium
M_1 = make_abcd(1, L1, 0, 1)
# right half-pass throught medium
M_2 = make_abcd(1, L2, 0, 1)
# output into air
M_out = make_abcd(1, 0, (1-n)/R2, n)

# full matrix - production of prev 4 matrices
M_4 = np.linalg.multi_dot([M_out, M_2, M_1, M_in])
print_abcd("M(4)", M_4)
compare_abcd("M_full == M_4", M_full, M_4)

print('----------------------------------')
print('Product of 2 matrices')

# input into medium and left half-pass
# ../help/matrix/ElemThickLens_left.png
M_lf = make_abcd(1 + L1*(n-1)/R1/n, L1/n, (n-1)/R1/n, 1/n)
print_abcd("M_lf", M_lf) 

# right half-pass and output into air
# ../help/matrix/ElemThickLens_right.png
M_rt = make_abcd(1, L2, (1-n)/R2, L2*(1-n)/R2 + n)
print_abcd("M_rt", M_rt) 

# matrix of plate - production of prev 2 matrices
M_2 = np.dot(M_rt, M_lf)
print_abcd("M(2)", M_2) 
compare_abcd("M_full == M_2", M_full, M_2)
compare_abcd("M_2 == M_4", M_2, M_4)
