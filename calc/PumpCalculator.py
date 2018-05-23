#!/usr/bin/env python
#
# Script produces validation data for
# PumpCalculator tests (../src/test/test_PumpCalculator.cpp).
#

from math import *
from cmath import *
from rezonator import *

def calcFront_Waist():
  print('\n----------------------------------')
  print('Waist:')
  def calc():
    z0 = pi * sqr(w0) / mi / wl
    Vs = mi * wl / pi / w0
    w = w0 * sqrt(1 + sqr(z/z0))
    R = z * (1 +  sqr(z0/z))
    q_inv = complex(1/R, wl/pi/sqr(w))
    q = 1 / q_inv
    print(q)

  wl = 980e-9

  w0 = 100e-6
  z = 0.1
  mi = 2
  calc()

  w0 = 150e-6
  z = 0.11
  mi = 1
  calc()


def calcFront_Front():
  print('\n----------------------------------')
  print('Front:')
  def calc():
    q_inv = complex(1/R, wl/pi/sqr(w))
    q = 1 / q_inv
    print(q)

  wl = 980e-9

  w = 300e-6
  R = 0.11
  calc()

  w = 400e-6
  R = 0.12
  calc()


def calcFront_RayVector():
  print('\n----------------------------------')
  print('Ray vector:')
  def calc():
    y2 = y1 + z * tan(V)
    print_res('y', y2)
    print_res('V', V)

  y1 = 0.001
  V = radians(5)
  z = 0.1
  calc()

  y1 = 0.0015
  V = radians(10)
  z = 0.09
  calc()


def calcFront_TwoSections():
  print('\n----------------------------------')
  print('Two section:')
  def calc():
    V = atan((y2 - y1)/z)
    print_res('y', y2)
    print_res('V', V)

  y1 = 0.001
  y2 = 0.002
  z = 0.1
  calc()

  y1 = 0.0015
  y2 = 0.0025
  z = 0.11
  calc()


def calcFront_InvComplex():
  print('\n----------------------------------')
  print('Inverted complex beam parameter:')
  q_t = 11 - 30j # 1/m
  print(1.0/q_t)
  q_s = 12 - 40j # 1/m
  print(1.0/q_s)


calcFront_Waist()
calcFront_Front()
calcFront_RayVector()
calcFront_TwoSections()
calcFront_InvComplex()
