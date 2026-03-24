import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("dataset2.txt")
x = data[:, 0]
y = data[:, 1]

A = np.vstack([x, np.ones(len(x))]).T

m, b = np.linalg.lstsq(A, y)[0]

yline = m * x + b

plt.scatter(x, y, color='blue')
plt.plot(x, yline, color='red')
plt.grid(True)
plt.show()