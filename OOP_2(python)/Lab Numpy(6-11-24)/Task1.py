import numpy as np

score = np.array([85, 90, 78, 92, 88])

#1.
f_a = score.astype('f')
print(f_a)
print("\n\n")

#2.
score1 = score.copy()
score1 += 5 
print(score1)
print("\n\n")

#3.
print(score.shape)
print(score.ndim)
print(score.itemsize)
print(score.dtype)

score.sort()
print(score)
print("\n\n")

#4.
print(np.where(score > 85))
print("\n\n")

#5.
print(score.min())
print(score.max())
print(score.std())
print(score.var())
print(score.sum())
print(score.mean())
print("\n\n")

#6.
print(score[::2])
print(score[-3:-1])
print(score[1:4])
