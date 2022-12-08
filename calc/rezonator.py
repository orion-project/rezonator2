#
# Common helper functions for other scripts.
#

import numpy as np
from math import *

def sqr(a):
  return a * a

# Returns matrix elements
# M should be numpy.ndarray
def get_abcd(M):
  A = M[0, 0]
  B = M[0, 1]
  C = M[1, 0]
  D = M[1, 1]
  return A, B, C, D


# Makes numpy matrix from elements
def make_abcd(A, B, C, D):
  return np.matrix([[A, B], [C, D]])


# Print ABCD-matrix
# M should be numpy.ndarray
def print_abcd(name, M):
  A, B, C, D = get_abcd(M)
  print("{}: {:.7f}, {:.7f}, {:.7f}, {:.7f}".format(name, A, B, C, D))

def print_abcd_c(name, M):
  A, B, C, D = get_abcd(M)
  print("{}: _C({:.7f},{:.7f}), _C({:.7f},{:.7f}), _C({:.7f},{:.7f}), _C({:.7f},{:.7f})" \
    .format(name, A.real, A.imag, B.real, B.imag, C.real, C.imag, D.real, D.imag))

# Compare two ABCD-matrices
def compare_abcd(id, M1, M2, epsilon = 1e-7):
  A1, B1, C1, D1 = get_abcd(M1)
  A2, B2, C2, D2 = get_abcd(M2)
  A = abs(A1 - A2) < epsilon
  B = abs(B1 - B2) < epsilon
  C = abs(C1 - C2) < epsilon
  D = abs(D1 - D2) < epsilon
  if not A or not B or not C or not D:
    print(id + ": (±{}) ** FAIL ** ({}, {}, {}, {})".format(epsilon, A, B, C, D))
  else:
    print(id + ': (±{}) ** OK **'.format(epsilon))

  
# Transformation of complex ROC by ray matrix    
# M should be numpy.ndarray
def transform_q(q1, M):
  A, B, C, D = get_abcd(M)
  return (A * q1 + B) / (C * q1 + D)


# Print named result value
def print_res(name, value):
  print("{}: {:.7g}".format(name, value))


# Calculate stability parameter from matrix
# M should be numpy.ndarray
def calc_stab(M):
  A, B, C, D = get_abcd(M)
  return (A+D)/2.0


# Calculate beamsize from matrix and wavelength
# M should be numpy.ndarray
def calc_w(M, wl, n):
  A, B, C, D = get_abcd(M)
  # B can be negative in a system with pure real matrices
  # and this gives a pure imaginary W while the system is stable.
  return sqrt(wl/n/pi * 2*abs(B) / sqrt(4 - sqr(A+D)))
