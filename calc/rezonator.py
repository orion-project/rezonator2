import numpy as np

# Returns matrix elements
# M should be numpy.ndarray
def get_abcd(M):
    A = M[0, 0]
    B = M[0, 1]
    C = M[1, 0]
    D = M[1, 1]
    return A, B, C, D


# Transformation of complex ROC by ray matrix    
# M should be numpy.ndarray
def transform_q(q1, M):
    A, B, C, D = get_abcd(M)
    return (A * q1 + B) / (C * q1 + D)
