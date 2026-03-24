import numpy as np
import matplotlib.pyplot as plt

A = np.array([[1,-2],[-4,1]])

eigenvalues, eigenvectors = np.linalg.eig(A)

print("Matrix:\n", A)
print("Eigenvalues:\n", eigenvalues)
print("Eigenvectors:\n", eigenvectors)

A1 = A[:, 0]
A2 = A[:, 1]
v1 = eigenvectors[:, 0]
v2 = eigenvectors[:, 1]
e1 = np.array([1,0])
e2 = np.array([0,1])

plt.figure(figsize=(10,10))

# Columns
plt.quiver(0, 0, A1[0], A1[1], angles = 'xy', scale_units = 'xy', scale = 1, color = 'red', label = 'A col 1')
plt.quiver(0, 0, A2[0], A2[1], angles = 'xy', scale_units = 'xy', scale = 1, color = 'red', label = 'A col 2')

# Eigenvectors
plt.quiver(0, 0, v1[0], v1[1], angles = 'xy', scale_units = 'xy', scale = 1, color = 'green', label = 'Eigenvector 1')
plt.quiver(0, 0, v2[0], v2[1], angles = 'xy', scale_units = 'xy', scale = 1, color = 'green', label = 'Eigenvector 2')

# Standard basis
plt.quiver(0, 0, e1[0], e1[1], angles = 'xy', scale_units = 'xy', scale = 1, color = 'blue', label = 'e1')
plt.quiver(0, 0, e2[0], e2[1], angles = 'xy', scale_units = 'xy', scale = 1, color = 'blue', label = 'e2')

plt.axhline(0, color = 'black', linewidth = 0.5)
plt.axvline(0, color = 'black', linewidth = 0.5)
plt.xlim(-5,5)
plt.ylim(-5,5)
plt.grid(True)
plt.gca().set_aspect('equal', adjustable='box')
plt.legend()

plt.show()