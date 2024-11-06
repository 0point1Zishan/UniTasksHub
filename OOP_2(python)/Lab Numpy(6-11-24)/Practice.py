import numpy as np
a = np.array([1, 2, 3])
print(a)
print(a.dtype)
print(a.ndim)
print(a.shape)
print(a.size)
print()

a = np.array([[1, 2, 3], [4, 5, 6]])
print(a)
print(a.dtype)
print(a.ndim)
print(a.shape)
print(a.size)
print()

a = np.array([[[1, 2, 3], [4, 5, 6]],[[1, 2, 3], [4, 5, 6]]])
print(a)
print(a.dtype)
print(a.ndim)
print(a.shape)
print(a.size)
print()

a = np.array(['a', 'b', 'c', 'd'])
print(a.itemsize)

a = np.array([1, 2, 3, 4])
print(a.itemsize)
print()



a = np.array([[1, 2, 3], [4, 5, 6]])

print(a > 5)
print()
print(a == 5)
print()
print(a < 5)
print()


# grades = np.array([[[1, 2, 3], [4, 10, 6]], [[1, 2, 3], [4, 5, 6]]])
grades = np.array([[1, 2, 3], [4, 10, 6]])
print(grades)

print("Max: ", grades.max())
print("Min: ", grades.min())
print("Sum: ", grades.sum())
print("std: ", grades.std())
print("mean: ", grades.mean())
print("Var: ", grades.var())
print("Column mean: ", grades.mean(axis = 0))
print("Row mean: ", grades.mean(axis = 1))
# print("Row Sum,: ", grades.Sum(axis = 0))
# print("Sum: ", grades.sum())
print()

a = np.array([[i for i in range(1, 31, 3)], [i for i in range(1, 31, 3)]])
print(a)


a = np.arange(1, 6)
print(a)

b = a.view()
print("Before: ")
print(b)
print(a)

b[0] = 100
print("After: ")
print(b)
print(a)

print(id(a))
print(id(b))

print()

c = np.arange(2, 10)
d = c
print("Before: ")
print(c)
print(d)

d[0] = 100
print("After: ")
print(b)
print(a)

print(id(c))
print(id(d))
