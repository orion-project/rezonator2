#!/usr/bin/env python

import numpy as np
from math import *
from rezonator import *

n0 = 1.5        # IOR
n2 = 0.2        # IOR gradient
L = 0.01        # Thickness of lens
h1 = 0.003      # Left half-pass
h2 = L - h1     # Right half-pass

print('----------------------------------')
print('Precalculated full matrix')
# ../help/matrix/ElemGrinLens.png
A = cos(L*sqrt(n2/n0))
B = sin(L*sqrt(n2/n0))/sqrt(n2*n0)
C = -sqrt(n2*n0)*sin(L*sqrt(n2/n0))
D = cos(L*sqrt(n2/n0))
M_full = make_abcd(A, B, C, D)
print_abcd("M(pre)", M_full)

print('----------------------------------')
print('Product of 2 matrices')

# input into medium and left half-pass
# ../help/matrix/ElemGrinLens_left.png
A1 = cos(h1*sqrt(n2/n0))
B1 = sin(h1*sqrt(n2/n0))/sqrt(n2*n0)
C1 = -sqrt(n2/n0)*sin(h1*sqrt(n2/n0))
D1 = cos(h1*sqrt(n2/n0))/n0
M_lf = make_abcd(A1, B1, C1, D1)
print_abcd("M_lf", M_lf)

# right half-pass and output into air
# ../help/matrix/ElemGrinLens_right.png
A2 = cos(h2*sqrt(n2/n0))
B2 = sqrt(n0/n2)*sin(h2*sqrt(n2/n0))
C2 = -sqrt(n2*n0)*sin(h2*sqrt(n2/n0))
D2 = n0*cos(h2*sqrt(n2/n0))
M_rt = make_abcd(A2, B2, C2, D2)
print_abcd("M_rt", M_rt)

# matrix of lens - production of prev 2 matrices
M_2 = np.dot(M_rt, M_lf)
print_abcd("M(2)", M_2) 
compare_abcd("M_full == M_2", M_full, M_2)
