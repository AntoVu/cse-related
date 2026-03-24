import numpy as np

def schmidt(A):
    A = np.array(A)
    row, col = A.shape
    mat = np.zeros((row, col))

    for i in range(col):
        icol = A[:, i]
        for j in range(i):
            jcol = mat[:, j]
            icol = icol - np.dot(jcol, icol) * jcol
        
        norm = np.linalg.norm(icol)
        mat[:, i] = icol / norm
    return mat

# Testing

"""
A = np.array([[1, 0, 1], [1, 1, 0], [0, 1, 1]])
Q = schmidt(A)

print("Q =\n", Q)
print("Orthonormality:\n", np.round(Q.T @ Q, 3))
"""
