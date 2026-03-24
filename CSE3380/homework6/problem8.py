import numpy as np

# Part a
N = 10
M = 4
X = np.random.randn(N, M)
print("X = \n", X)

# Part b
cov_matrix = X.T @ X
eigenvalues, eigenvectors = np.linalg.eig(cov_matrix)
print("Eigenvalues:\n", eigenvalues)
print("Eigenvectors:\n", eigenvectors)

# Part c
projected_eigenvectors = []
for i in range(eigenvectors.shape[1]):
    v = eigenvectors[:, i]
    u = X @ v
    norm = np.linalg.norm(u)
    u = u/norm
    projected_eigenvectors.append(u)
projected_eigenvectors = np.array(projected_eigenvectors).T
print("Projected eigenvectors:\n", projected_eigenvectors)

# Part d
U, S, Vt = np.linalg.svd(X, full_matrices=False)
print("U in SVD:\n", U)
print("∑ in SVD:\n", S)
print("V^T in SVD:\n", Vt)

# Part e
for i in range(U.shape[1]):
    print("\nColumn", i)
    print("U[:, i]", U[:, i])
    print("Projected eigenvector =", projected_eigenvectors[:, i])

"""
Observations
============
The projected eigenvector after normalizing matches to the U matrix except that their sign is flipped
Singular values are also equal to the sqrts of the eigenvalues
"""