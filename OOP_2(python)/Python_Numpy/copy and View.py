import numpy as np
a = np.array([1, 2, 3, 4, 5])
b = a.copy()

print(a)
print(b)

a[2] = 100
print("After change of a:")
print(a)
print(b, end = "\n\n")


b[3] = 500
print("After change of b:")
print(b)
print(a)