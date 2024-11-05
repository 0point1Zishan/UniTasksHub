import numpy as np
a = np.array(42)
b = np.array([1, 2, 3, 4, 5])
c = np.array([[1, 2, 3], [4, 5, 6]])
d = np.array([[[1,2], [3, 4]],[[5, 6], [7, 8]]])

print(a)
print(a.ndim)
print(b)
print(b.ndim)
print(c)
print(c.ndim)
print(d)
print(d.ndim)

arr = np.array([1, 2, 3, 4], ndmin = 5)
print(arr)
print("Number of dimensions:", arr.ndim)

e = np.array([1,2,3,4,5,6,7,8,9], ndmin = 3)
print(e)
print("Number of dimensions:", e.ndim)
