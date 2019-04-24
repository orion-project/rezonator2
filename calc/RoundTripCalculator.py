#!/usr/bin/env python
#
# Script produces validation data for
# RoundTripCalculator tests (../src/test/test_RoundTripCalculator.cpp).
# Matrices are from test schema ../bin/test_files/test_mult_matrix.rez.
#

import numpy as np
from rezonator import *

def mult_matrices():
  print('\nMult matrices\n')
  L1_t = make_abcd(1, 0.05, 0, 1)
  L1_s = L1_t
  
  F1_t = make_abcd(1, 0, -10.1543, 1)
  F1_s = make_abcd(1, 0, -9.84808, 1)
  
  L2_t = make_abcd(1, 0.075, 0, 1)
  L2_s = L2_t
  
  Cr1_t = make_abcd(1, 0.0650857, 0, 1)
  Cr1_s = make_abcd(1, 0.0676818, 0, 1)

  L3_t = make_abcd(1, 0.1, 0, 1)
  L3_s = L3_t

  M_t = np.linalg.multi_dot([L3_t, Cr1_t, L2_t, F1_t, L1_t])
  M_s = np.linalg.multi_dot([L3_s, Cr1_s, L2_s, F1_s, L1_s])
  
  print_abcd('M_t', M_t)
  print_abcd('M_s', M_s)


def mult_matrices_interface_brewster():
  print('\nMult matrices (brewster interface)\n')
  L1 = make_abcd(1, 0.1, 0, 1)
  L2 = make_abcd(1, 0.1, 0, 1)

  n1 = 1
  n2 = 2
  L_cr = 0.1
  Cr_t = make_abcd(1, L_cr/n2/n2/n2, 0, 1)
  Cr_s = make_abcd(1, L_cr/n2, 0, 1)
  M_t = np.linalg.multi_dot([Cr_t, L1, Cr_t, L2])
  M_s = np.linalg.multi_dot([Cr_s, L1, Cr_s, L2])
  print_abcd('M_t', M_t)
  print_abcd('M_s', M_s)

  Cr = make_abcd(1, L_cr, 0, 1)
  Cr_in_t = make_abcd(n2/n1, 0, 0, (n1/n2)*(n1/n2))
  Cr_in_inv_t = make_abcd(n1/n2, 0, 0, (n2/n1)*(n2/n1))
  Cr_out_t = make_abcd(n1/n2, 0, 0, (n2/n1)*(n2/n1))
  Cr_out_inv_t = make_abcd(n2/n1, 0, 0, (n1/n2)*(n1/n2))
  Cr_in_s = make_abcd(1, 0, 0, n1/n2)
  Cr_in_inv_s = make_abcd(1, 0, 0, n2/n1)
  Cr_out_s = make_abcd(1, 0, 0, n2/n1)
  Cr_out_inv_s = make_abcd(1, 0, 0, n1/n2)
  M_t = np.linalg.multi_dot([Cr_out_t, Cr, Cr_in_t, L1, Cr_in_inv_t, Cr, Cr_out_inv_t, L2])
  M_s = np.linalg.multi_dot([Cr_out_s, Cr, Cr_in_s, L1, Cr_in_inv_s, Cr, Cr_out_inv_s, L2])
  print_abcd('M_t', M_t)
  print_abcd('M_s', M_s)


mult_matrices()
mult_matrices_interface_brewster()
