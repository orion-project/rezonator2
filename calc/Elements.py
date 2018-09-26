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

print('\n----------------------------------')
print('NormalInterface\n')
# ../img/matrices/ElemNormalInterface.png
n1 = 1.6
n2 = 3.7
M = make_abcd(1, 0, 0, n1/n2)
M_inv = make_abcd(1, 0, 0, n2/n1)
print_abcd('M', M)
print_abcd('M_inv', M_inv)

print('\n----------------------------------')
print('BrewsterInterface\n')
# ../img/matrices/ElemBrewsterInterface.png
n1 = 1.6
n2 = 3.7
M_t = make_abcd(n2/n1, 0, 0, pow(n1/n2, 2))
M_s = make_abcd(1, 0, 0, n1/n2)
M_inv_t = make_abcd(n1/n2, 0, 0, pow(n2/n1, 2))
M_inv_s = make_abcd(1, 0, 0, n2/n1)
print_abcd('M_t', M_t)
print_abcd('M_s', M_s)
print_abcd('M_inv_t', M_inv_t)
print_abcd('M_inv_s', M_inv_s)

print('\n----------------------------------')
print('TiltedInterface\n')
# ../img/matrices/ElemTiltedInterface.png
n1 = 1.6
n2 = 3.7
a = radians(15)          # angle in n1
b = asin((n1/n2)*sin(a)) # angle in n2
M_t = make_abcd(cos(b)/cos(a), 0, 0, (n1/n2)*(cos(a)/cos(b)))
M_s = make_abcd(1, 0, 0, n1/n2)
M_t_inv = make_abcd(cos(a)/cos(b), 0, 0, (n2/n1)*(cos(b)/cos(a)))
M_s_inv = make_abcd(1, 0, 0, n2/n1)
print_abcd('M_t', M_t)
print_abcd('M_s', M_s)
print_abcd('M_inv_t', M_t_inv)
print_abcd('M_inv_s', M_s_inv)
print('')
b = radians(15)          # angle in n2
a = asin((n2/n1)*sin(a)) # angle in n1
M_t = make_abcd(cos(b)/cos(a), 0, 0, (n1/n2)*(cos(a)/cos(b)))
M_s = make_abcd(1, 0, 0, n1/n2)
M_t_inv = make_abcd(cos(a)/cos(b), 0, 0, (n2/n1)*(cos(b)/cos(a)))
M_s_inv = make_abcd(1, 0, 0, n2/n1)
print_abcd('M_t', M_t)
print_abcd('M_s', M_s)
print_abcd('M_inv_t', M_t_inv)
print_abcd('M_inv_s', M_s_inv)

print('\n----------------------------------')
print('SphericalInterface\n')
# ../img/matrices/ElemSphericalInterface.png
n1 = 1.7
n2 = 3.8
R = 0.9
M = make_abcd(1, 0, (n2-n1)/R/n2, n1/n2)
M_inv = make_abcd(1, 0, (n1-n2)/-R/n1, n2/n1)
print_abcd('M', M)
print_abcd('M_inv', M_inv)
