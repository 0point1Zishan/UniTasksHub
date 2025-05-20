# Tuple is immutable
t = (1, 2, 3, 4, 5)
print(t)
print(t[2])
print(type(t))


print("\n")
t = (1,) 
print(type(t))

t = (1)
print(type(t))


# Slicing
tu = (1, 2, 3, 4, 5, 2, 6, 7,2)
print(tu[3: len(tu)])


## Methods:
print(tu.index(3))

print(tu.count(2))