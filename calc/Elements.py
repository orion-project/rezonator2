#!/usr/bin/env python

import numpy as np
from math import *
from rezonator import *

print('\n----------------------------------')
print('CurveMirror\n')
# ../img/matrices/ElemCurveMirror.png
R = 0.1         # ROC
a = radians(15) # incident angle
Mt = make_abcd(1, 0, -2 / (R * cos(a)), 1)
Ms = make_abcd(1, 0, -2 * cos(a) / R, 1)
print_res('Incident angle', a)
print_abcd('T', Mt)
print_abcd('S', Ms)

print('\n----------------------------------')
print('ThinLens\n')
# ../img/matrices/ElemThinLens.png
F = 0.1         # focal range
a = radians(15) # incident angle
Mt = make_abcd(1, 0, -1 / (F * cos(a)), 1)
Ms = make_abcd(1, 0, -1 * cos(a) / F, 1)
print_res('Incident angle', a)
print_abcd('T', Mt)
print_abcd('S', Ms)

print('\n----------------------------------')
print('EmptyRange\n')
# ../img/matrices/ElemEmptyRange.png
L = 0.088
L1 = L * 0.2
L2 = L - L1
print_res('Subrange', L1)
M_full = make_abcd(1, L, 0, 1)
M_lf = make_abcd(1, L1, 0, 1)
M_rt = make_abcd(1, L2, 0, 1)
M_2 = np.dot(M_rt, M_lf)
print_abcd('M_full', M_full)
print_abcd('M_lf', M_lf)
print_abcd('M_rt', M_rt)
print_abcd('M_2', M_2)
compare_abcd("M_full == M_2", M_full, M_2)

print('\n----------------------------------')
print('MediumRange\n')
# ../img/matrices/ElemMediaRange.png
L = 0.088
n = 1.2
L1 = L * 0.2
L2 = L - L1
print_res('Subrange', L1)
print_res('Optical path', L * n)
M_full = make_abcd(1, L, 0, 1)
M_lf = make_abcd(1, L1, 0, 1)
M_rt = make_abcd(1, L2, 0, 1)
M_2 = np.dot(M_rt, M_lf)
print_abcd('M_full', M_full)
print_abcd('M_lf', M_lf)
print_abcd('M_rt', M_rt)
print_abcd('M_2', M_2)
compare_abcd("M_full == M_2", M_full, M_2)