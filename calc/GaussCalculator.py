#!/usr/bin/env python

# Script produces validation data for
# Gauss Calculator Tool tests (../src/test/test_GaussCalculator.cpp).
# Tool explanation with formulas is in GaussCalculator.md file.

from math import *
from cmath import *
from rezonator import *

print('\n----------------------------------')
print('Change waist:')
wl = 980e-9  # wavelength
z = 0.1      # axial distance
mi = 2       # beam quality

print('\nLock waist:')
w0 = 100e-6 # waist radius

z0 = pi * sqr(w0) / mi / wl
Vs = mi * wl / pi / w0
w = w0 * sqrt(1 + sqr(z/z0))
R = z * (1 +  sqr(z0/z))
q_inv = complex(1/R, wl/pi/sqr(w))
q = 1 / q_inv

print_res('MI', mi)
print_res('wl', wl)
print_res('w0', w0)
print_res('z0', z0)
print_res('Vs', Vs)
print_res('Vs(deg)', degrees(Vs))
print_res('z', z)
print_res('w', w)
print_res('R', R)
print_res('q_inv', q_inv)
print_res('q', q)

print('\nLock font:')
w0 = 75e-6 # waist radius

z = R*(1 - sqr(w0 / w))
z0 = sqrt(z * (R - z))
mi = pi * sqr(w0) / wl / z0
Vs = mi * wl / pi / w0

print_res('MI', mi)
print_res('wl', wl)
print_res('w0', w0)
print_res('z0', z0)
print_res('Vs', Vs)
print_res('Vs(deg)', degrees(Vs))
print_res('z', z)
print_res('w', w)
print_res('R', R)
print_res('q_inv', q_inv)
print_res('q', q)

del mi
del wl
del w0
del z0
del Vs
del z
del w
del R
del q_inv
del q

print('\n----------------------------------')
print('Change Rayleigh length:')

wl = 1.1e-6  # wavelength
w0 = 100e-6  # waist radius
z = 0.1      # axial distance

print('\nLock waist:')
z0 = 0.012

mi = pi * sqr(w0) / wl / z0
Vs = mi * wl / pi / w0
w = w0 * sqrt(1 + sqr(z/z0))
R = z * (1 +  sqr(z0/z))
q_inv = complex(1/R, wl/pi/sqr(w))
q = 1 / q_inv

print_res('MI', mi)
print_res('wl', wl)
print_res('w0', w0)
print_res('z0', z0)
print_res('Vs', Vs)
print_res('Vs(deg)', degrees(Vs))
print_res('z', z)
print_res('w', w)
print_res('R', R)
print_res('q_inv', q_inv)
print_res('q', q)

print('\nLock font:')
z0 = 0.016

print('\nFar zone:')
z = R/2.0 + sqrt(sqr(R) - 4*sqr(z0))/2.0
print('z={}, z0={}, w={}'.format(z,z0,w))
#w0 = sqrt(sqr(w) / (1 + sqr(z0 / z)))
w0 = w / sqrt((1 + sqr(z0 / z)))
mi = pi * sqr(w0) / wl / z0
Vs = mi * wl / pi / w0

print_res('MI', mi)
print_res('wl', wl)
print_res('w0', w0)
print_res('z0', z0)
print_res('Vs', Vs)
print_res('Vs(deg)', degrees(Vs))
print_res('z', z)
print_res('w', w)
print_res('R', R)
print_res('q_inv', q_inv)
print_res('q', q)
'''
print('\nNear zone:')
z = R/2.0 - sqrt(sqr(R) - 4*sqr(z0))/2.0
w0 = sqrt(sqr(w) / (1 + sqr(z0 / z)))
mi = pi * sqr(w0) / wl / z0
Vs = mi * wl / pi / w0

print_res('MI', mi)
print_res('wl', wl)
print_res('w0', w0)
print_res('z0', z0)
print_res('Vs', Vs)
print_res('Vs(deg)', degrees(Vs))
print_res('z', z)
print_res('w', w)
print_res('R', R)
print_res('q_inv', q_inv)
print_res('q', q)
'''