#!/usr/bin/env python

import numpy as np
from math import *
from rezonator import *

n = 1.5                # IOR
a = radians(15)        # incident angle
b = asin(sin(a)/n)     # angle inside medium
L = 0.0045             # length of plate
L_axis = L / cos(b)    # axial length of plate
L1 = L_axis * 0.2
L2 = L_axis - L1

print('----------------------------------')
print_res('Incident angle', a)
print_res('Effective length', L_axis)
print_res('Optical path', L_axis * n)
print_res('Subrange', L1)

print('----------------------------------')
print('Precalculated full matrix')
# ../img/matrices/ElemTiltedPlate.png
s = sqr(n) - sqr(sin(a))
M_t_full = make_abcd(1, L * sqr(n) * (1 - sqr(sin(a))) / sqrt(pow(s, 3)), 0, 1)
M_s_full = make_abcd(1, L / sqrt(s), 0, 1)
print_abcd("M_t(pre)", M_t_full)
print_abcd("M_s(pre)", M_s_full)

print('----------------------------------')
print('Product of 4 matrices')

# input into medium
M_in_t = make_abcd(cos(b)/cos(a), 0, 0, 1/n * cos(a)/cos(b))
M_in_s = make_abcd(1, 0, 0, 1/n)
# left half-pass throught medium
M_1 = make_abcd(1, L1, 0, 1)
# right half-pass throught medium
M_2 = make_abcd(1, L2, 0, 1)
# output into air
M_out_t = make_abcd(cos(a)/cos(b), 0, 0, n * cos(b)/cos(a))
M_out_s = make_abcd(1, 0, 0, n)

# full matrix - production of prev 4 matrices
M_t_4 = np.linalg.multi_dot([M_out_t, M_2, M_1, M_in_t])
M_s_4 = np.linalg.multi_dot([M_out_s, M_2, M_1, M_in_s])
print_abcd("M_t(4)", M_t_4)
print_abcd("M_s(4)", M_s_4)
compare_abcd("M_t_full == M_t_4", M_t_full, M_t_4)
compare_abcd("M_s_full == M_s_4", M_s_full, M_s_4)

print('----------------------------------')
print('Product of 2 matrices')

# input into medium and left half-pass
# ../img/matrices/ElemTiltedPlate_left.png
M_lf_t = make_abcd(cos(b)/cos(a), L1/n * cos(a)/cos(b), 0, 1/n * cos(a)/cos(b))
M_lf_s = make_abcd(1, L1/n, 0, 1/n)
print_abcd("M_lf_t", M_lf_t) 
print_abcd("M_lf_s", M_lf_s)

# right half-pass and output into air
# ../img/matrices/ElemTiltedPlate_right.png
M_rt_t = make_abcd(cos(a)/cos(b), L2 * cos(a)/cos(b), 0, n * cos(b)/cos(a))
M_rt_s = make_abcd(1, L2, 0, n)
print_abcd("M_rt_t", M_rt_t) 
print_abcd("M_rt_s", M_rt_s)

# full matrix - production of prev 2 matrices
M_t_2 = np.dot(M_rt_t, M_lf_t)
M_s_2 = np.dot(M_rt_s, M_lf_s)
print_abcd("M_t(2)", M_t_2) 
print_abcd("M_s(2)", M_s_2)
compare_abcd("M_t_full == M_t_2", M_t_full, M_t_2)
compare_abcd("M_s_full == M_s_2", M_s_full, M_s_2)
