import numpy as np

a = np.array([1, 2, 3, 4, 5])
# c = a.reshape(2, 2, -1)
# print(c)

# a = np.array([1, 2, 3, 4, 5, 6, 7, 8])
# print(a.reshape(2, 4).base)

a = np.array([[1, 2, 3, 4],[ 5, 6, 7, 8]])
b = a.reshape(-1)
print(b)

a = np.array([[1, 2, 3], [5, 6, 7]])
b = a.reshape(6)
print(b)


