#!/usr/bin/env python

import numpy as np
from math import *
from rezonator import *

n0 = 1.5        # IOR
n2 = 0.3        # IOR gradient
L = 0.01        # Thickness of lens
h1 = 0.002      # Left half-pass
h2 = L - h1     # Right half-pass
g = sqrt(n2/n0)

print('----------------------------------')
print('GRIN medium: Precalculated full matrix')
# ../help/matrix/ElemGrinMedium.png
M_GM_full = make_abcd(cos(L*g), sin(L*g)/g, -g*sin(L*g), cos(L*g))
print_abcd("M(pre)", M_GM_full)

print('----------------------------------')
print('GRIN medium: Product of 2 matrices')

# left half-pass
M_GM_lf = make_abcd(cos(h1*g), sin(h1*g)/g, -g*sin(h1*g), cos(h1*g))
print_abcd("M_lf", M_GM_lf)

# right half-pass
M_GM_rt = make_abcd(cos(h2*g), sin(h2*g)/g, -g*sin(h2*g), cos(h2*g))
print_abcd("M_rt", M_GM_rt)

# matrix of medium - production of prev 2 matrices
M_GM_2 = np.dot(M_GM_rt, M_GM_lf)
print_abcd("M(2)", M_GM_2) 
compare_abcd("M_full == M_2", M_GM_full, M_GM_2)

print('----------------------------------')
print('GRIN lens: Precalculated full matrix')
# ../help/matrix/ElemGrinLens.png
A = cos(L*sqrt(n2/n0))
B = sin(L*sqrt(n2/n0))/sqrt(n2*n0)
C = -sqrt(n2*n0)*sin(L*sqrt(n2/n0))
D = cos(L*sqrt(n2/n0))
M_GL_full = make_abcd(A, B, C, D)
print_abcd("M(pre)", M_GL_full)

print('----------------------------------')
print('GRIN lens: Product of 2 matrices')

# input into medium and left half-pass
# ../help/matrix/ElemGrinLens_left.png
A1 = cos(h1*sqrt(n2/n0))
B1 = sin(h1*sqrt(n2/n0))/sqrt(n2*n0)
C1 = -sqrt(n2/n0)*sin(h1*sqrt(n2/n0))
D1 = cos(h1*sqrt(n2/n0))/n0
M_GL_lf = make_abcd(A1, B1, C1, D1)
print_abcd("M_lf", M_GL_lf)

# right half-pass and output into air
# ../help/matrix/ElemGrinLens_right.png
A2 = cos(h2*sqrt(n2/n0))
B2 = sqrt(n0/n2)*sin(h2*sqrt(n2/n0))
C2 = -sqrt(n2*n0)*sin(h2*sqrt(n2/n0))
D2 = n0*cos(h2*sqrt(n2/n0))
M_GL_rt = make_abcd(A2, B2, C2, D2)
print_abcd("M_rt", M_GL_rt)

# matrix of lens - production of prev 2 matrices
M_GL_2 = np.dot(M_GL_rt, M_GL_lf)
print_abcd("M(2)", M_GL_2) 
compare_abcd("M_full == M_2", M_GL_full, M_GL_2)

# matrix of lens - production of 3 matrices
M_out = make_abcd(1, 0, 0, n0)
M_in = make_abcd(1, 0, 0, 1/n0)
M_GL_3 = np.dot(np.dot(M_out, M_GM_full), M_in)
print_abcd("M(3)", M_GL_3) 
compare_abcd("M_full == M_3", M_GL_full, M_GL_3)
