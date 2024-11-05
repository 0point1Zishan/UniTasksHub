import numpy as np
a = np.array([1, 2, 3, 4])
print(a.dtype, end = "\n\n")

a = np.array(["apple", "aanana", "orange", "orange"])
print(a.dtype, end = "\n\n")

a = np.array([1, 2, 3, 4], dtype = 'S')
print(a)
print(a.dtype)
print()


a = np.array([1, 2, 3, 4], dtype = 'i4')
print(a)
print(a.dtype, end = "\n\n")

a = np.array([1, 2, 3, 4], dtype = 'i8')
print(a)
print(a.dtype, end = "\n\n")

a = np.array(['a', 'a', 'a', 'a'], dtype = 'U')
print(a)
print(a.dtype, end = "\n\n")


a = np.array([1, 2, 3, 4], dtype='i2')
print(a)
print(a.dtype, end="\n\n")

a = np.array([1.1, 1.2, 1.3])
temp_a = a.astype('i4')
print(temp_a)
print(temp_a.dtype, end = "\n\n")

a = np.array([1, 0, 3, -4])
b = a.astype(bool)
print(b)
print(b.dtype)
