import numpy as np
arr = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
print(arr[1:5], end = '\n\n')

print(arr[3:], end = '\n\n')

print(arr[-4: -1], end = '\n\n')

print(arr[1:5:2], end = '\n\n')

print(arr[::3], end = '\n\n')


a = np.array([[1, 2, 3, 4, 5, 6, 7], [8, 9, 10, 11, 12, 13, 14]])
print(a[0, 1:4], end = "\n\n")

print(a[0:2, 2], end = "\n\n")

print(a[0:2, 1:4])