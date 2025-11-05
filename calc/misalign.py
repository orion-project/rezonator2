import numpy as np

# M_back: R = 30 mm, α = 0°
# L_foc: L = 57 mm
# M_foc: R = 50 mm, α = 7°, dYt = 1 mm, dVt = 0.0175
# L: L = 300 mm Folding range
# λ = 1000 nm 

M_out = np.matrix([[1, 0], [0, 1]])
M_back = np.matrix([[1, 0], [-66.66666666666667, 1]])
L = np.matrix([[1, 0.3], [0, 1]])
M_foc = np.matrix([[1, 0], [-40.30039301835393, 1]])
L_foc = np.matrix([[1, 0.057], [0, 1]])

M0 = M_out * L * M_foc * L_foc * M_back * L_foc * M_foc * L
print(M0)

M_out = np.matrix([[1, 0, 0], [0, 1, 0], [0, 0, 1]])
M_back = np.matrix([[1, 0, 0], [-66.66666666666667, 1, 0], [0, 0, 1]])
L = np.matrix([[1, 0.3, 0], [0, 1, 0], [0, 0, 1]])
M_foc = np.matrix([[1, 0, 0.001], [-40.30039301835393, 1, 0.0175], [0, 0, 1]])
L_foc = np.matrix([[1, 0.057, 0], [0, 1, 0], [0, 0, 1]])

M0 = M_out * L * M_foc * L_foc * M_back * L_foc * M_foc * L
print(M0)

E = M0[0, 2]
F = M0[1, 2]
print('Effective misalignment:\n', E, F)

A = M0[0, 0]
B = M0[0, 1]
C = M0[1, 0]
D = M0[1, 1]
E0 = ((1 - D)*E + B*F) / (2 - A - D)
F0 = (C*E + (1 - A)*F) / (2 - A - D)
print('Position of effective optical axis:\n', E0, F0)
