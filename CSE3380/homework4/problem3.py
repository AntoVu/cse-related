import sympy as sp

# P matrix
P = sp.Matrix([[4,-9,5],[-3,-1,6],[9,-2,-6]])
# B matrix
B = sp.Matrix([[0,4,3],[-1,5,3],[3,-4,-6]])

# Invert P
P_inv = P.inv()
# A = B * P inverted
A = B * P_inv

sp.pprint("Matrix A:")
sp.pprint(A)