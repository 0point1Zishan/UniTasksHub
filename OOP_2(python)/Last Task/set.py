a = {1, 3, 5, 8, 3, 7}
b = {0, False, 1, 5}


#1. Print a and b
print(a)
print(b)

#3. Print length and type
print(f"Length of a: {len(a)}")
print(f"Length of b: {len(b)}")
print(f"Type of a: {type(a)}")
print(f"Type of b: {type(b)}")

#3. add a new element 10 in set a
a.add(10)
print(a)

#4. Remove 8 form the set a
a.remove(8)
print(a)
print()

#5. Perform union, intersection, difference, symmetric difference, issubset operation on set and set b
print(a.union(b))
print(a.intersection(b))
print(a.difference(b))
print(b.difference(a))
print(b.symmetric_difference(a))
print(a.issubset(b))
print(b.issubset(a))
print()

#6. Join a new list [2, 3, 4] with set a
print(a)
newList = [2, 3, 4]
a.update(newList)
print(a)