# Converts a vector between standard and another basis

# Approach:
# Go from coordinates in basis B to standard coordinates
# Go from standard coordinates to coordinates in basis B

import sympy as sp

def basis_change(B, v):
  # Given matrix B and vector v returns the coordinates of v in the basis of B
  B_invert = B.inv()
  return B_invert * v

# Make matrix
B = sp.Matrix([[0,-4,6],[-1,0,6],[-1,0,3]])

# Make vectors
x = sp.Matrix([-18,8,5])
xB = sp.Matrix([-2,6,1])

# Transforming from standard to B
x_to_B = basis_change(B, x)

# Transforming from B to standard
xB_to_standard = B * xB

# Print result
print("Vector x in basis B coordinates:")
sp.pprint(x_to_B)
print("\nVector [x]_B in standard coordinates:")
sp.pprint(xB_to_standard)