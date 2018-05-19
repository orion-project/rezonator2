#!/usr/bin/env python
#
# Script produces validation data for
# BeamCalculator tests (../src/test/test_BeamCalculator.cpp).
#

from math import *
from cmath import *
from rezonator import *

def calcVector():
  print('\n----------------------------------')
  print('Vector:')

  # Input vector
  Y1 = 0.01
  V1 = 0.05

  # Matricex of schema ../bin/test_files/test_mult_matrix.rez
  A = -1.4379022
  B = 0.1681906
  C = -10.1543000
  D = 0.4922850

  # Output beam
  Y2 = Y1 * A + V1 * B
  V2 = Y1 * C + V1 * D
  R = Y2 / sin(V2)

  print_res('Radius', Y2)
  print_res('Half angle', V2)
  print_res('Wavefront ROC', R)


def calcGauss():
  print('\n----------------------------------')
  print('Gauss:')

  # Input beam
  wl = 980e-9
  z = 0.1
  mi = 2
  w0 = 100e-6
  z0 = pi * sqr(w0) / mi / wl
  Vs = mi * wl / pi / w0
  w = w0 * sqrt(1 + sqr(z/z0))
  R = z * (1 +  sqr(z0/z))
  q_inv = complex(1/R, wl/pi/sqr(w))
  q1 = 1 / q_inv
  print_res('Q_in', q1)

  # Matricex of schema ../bin/test_files/test_mult_matrix.rez
  A = -1.3899498
  B = 0.1731843
  C = -9.8480800
  D = 0.5075960

  # Output beam
  q2 = (A * q1 + B) / (C * q1 + D)
  q_inv = 1 / q2
  R = 1.0 / q_inv.real
  w = sqrt(wl / pi / q_inv.imag)
  tmp = pow(w, 4) * sqr(pi)
  z = tmp * R / (sqr(mi * wl * R) + tmp)
  z0 = sqrt(z * (R - z))
  w0 = sqrt(mi * z0 * wl / pi)
  Vs = mi * wl / pi / w0

  print_res('Radius', w)
  print_res('Half angle', Vs)
  print_res('Wavefront ROC', R)


calcVector()
calcGauss()
