a = (1, 3, 5, 7, 4)  # Tuple

# Length, type:
print("Length of tuple a:", len(a))
print("Type of a:", type(a))

# Access and change:
print("Second to last element of tuple a:", a[-2])
print("Third element of tuple a:", a[2])

# Convert tuple to list
b = list(a)
b[-3] = 50

# Print modified list b
print("Modified list b:", b)

# Iterate over a subset of list b
for i in range(-4, -1):
    print(b[i], end=" ")

print()

# Add value to list
b.append(100)
print("List b after append:", b)

# Insert value at index 2
b.insert(2, 400)
print("List b after insert:", b)

# Remove last element
b.pop()
print("List b after pop:", b)

# Add tuple to list
c = (2, 4, 6)
b.append(c)
print("List b after appending tuple c:", b)

# Iterate and print elements of list b
for i in b:
    if i == 7:
        break
    print(i, end=", ")

print()

# Sets

a = {1, 3, 5, 8, 3, 2}
print("Set a:", a)
b = {0, False, 1, 5}
print("Set b:", b)

print("Length of set a:", len(a))
print("Length of set b:", len(b))

# Add element to set
a.add(10)
print("Set a after adding 10:", a)

# Remove element from set
a.remove(8)
print("Set a after removing 8:", a)

# Union of sets
c = a.union(b)
print("Union of sets a and b:", c)

# Intersection of sets
c = a.intersection(b)
print("Intersection of sets a and b:", c)

# Difference of sets
c = a.difference(b)
print("Difference of sets a and b:", c)

# Adding a list to a set (converting list to set first)
d = [2, 3, 4]
a = a.union(set(d))
print("Set a after union with list d:", a)

# Iterate over set a
for i in a:
    print(i, end=", ")

print()
if 3 in a:
    print("3 is present")

# Dictionaries

employee = {
    "name": "A",
    "age": 40,
    "type": {"developer": ["ios", "android"]},
    "permanent": False,
    "Salary": 30000,
    100: (1, 2, 3),
    4.5: {6, 5, True, 7, 0},
    True: 1
}

print("Type of employee dictionary:", type(employee))
print("Length of employee dictionary:", len(employee))

# Access nested dictionary elements
print("Second developer type:", employee["type"]["developer"][1])
print("Length of developer types:", len(employee["type"]["developer"]))
print("Size of 'type' dictionary:", len(employee["type"]))

# Modify a dictionary value
employee["permanent"] = True
print("Employee dictionary after modifying 'permanent':", employee)

# Add a new key-value pair
employee["gender"] = "male"
print("Employee dictionary after adding 'gender':", employee)

# Access and print elements from the set stored at key 4.5
print("Values for key 4.5:", employee[4.5])

# Iterate over the set at key 4.5
for i in employee[4.5]:
    print(i, end=" ")
print()

# Delete a key-value pair
del employee["age"]
print("Employee dictionary after deleting 'age':", employee)

# Print all values and keys
print("Values in employee dictionary:", employee.values())
print("Keys in employee dictionary:", employee.keys())

# Iterate and print values
for i in employee.values():
    print(i, end=" ")
print()

# Iterate and print keys
for i in employee.keys():
    print(i, end=" ")
print()

# Corrected loop for key-value pairs
print("Dictionary loop")
for key in employee:
    print(f"{key}'s value is: {employee[key]}")
print()

# Print value for boolean key True
print("Value for key True:", employee[True])