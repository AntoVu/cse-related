# Finds dimension and basis for the set

# Approach:
# Use sympy to make matrix
# Get the vectors and put them into a 4x3 matrix
# Use rank() to get the dimension
# Use columnspace() to get basis in list of list format
# Use for each loop to get lists out of ^ list right there

import sympy as sp

# Make matrix
matrix = sp.Matrix([[1,0,1],[1,1,-1],[4,2,0],[3,-1,-1]])

# Get dimension
dimension = matrix.rank()

# Get basis
basis = matrix.columnspace()

# Print result
print(f"Dimension of the set: {dimension}")
print("Basis vectors:")
for vector in basis:
  print(vector)