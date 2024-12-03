a = [1, 3, 5, 7, 4]

# 1. Access a[-2], a[2]
print(a[-2])
print(a[2])

# Find length and type
print(len(a))
print(type(a))

# 2. Change:
a[-3] = 50
for x in a[2:4]:
    print(x, end=" ")
print()

# 3.
    #add 100 in last index
a.append(100)
    #add 200 in index 2
a.insert(2, 200)
print(a)

# 4.
    #remove last element
a.pop()
print(a)
    #remove element in index 1
a.pop(1)
print(a)

#5. Join a new list
a.extend([2, 4, 6])
print(a)

#6. Copy all values in a new list b
b = a.copy()
print(b)

#7. Sort the element of b:
b.sort()
print(b)

#8. Print all the elements using loop and break if get 5
a[-3] = 5
for x in a:
    if(x == 5):
        break
    print(x, end=" ")
print()

print(f"The Largest element of a is: {max(a)}")