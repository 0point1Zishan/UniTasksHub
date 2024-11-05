import numpy as np

#! 1-D Array;
print("1-D Array:")
oneD = np.array([1,2,3,4,5])
print(oneD[0])
print(oneD[1])
print(oneD[2] + oneD[3])
print()


#! 2-D Array:
print("2-D Array:")
twoD = np.array([[1, 2], [3, 4]])
print(twoD[1][0])
print()


#! 3-D Array:
print("3-D Array:")
thrD = np.array([[ [1, 2], [3, 4]], [[5, 6], [7, 8]]])
print(thrD[1, 1, 1])
print()


#! Negative Indexing:
a = np.array([[1,2,3,4,5], [6,7,8,9,10]])
print("Last element of a: ", a[0, -5])
print(a[1][-5])