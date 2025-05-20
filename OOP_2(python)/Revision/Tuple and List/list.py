# List is mutable but tuple and sting is immutable
marks = [90, 92.5, 99.5, 98, 70]
print(marks)
print(type(marks))


print(marks[0])
print(marks[1])
print(len(marks))

marks[3] = "Zishan"
print(marks)

#marks[3][0] = 'A'   # Not possible
print(marks[2: len(marks)])


# Methods:
list = [2, 1, 3, 1]
list.append(4)      # add at the end    
print(list)

list.insert(1, 5)   # insert in specific index
print(list)

list.sort()         # ascending order sort
print(list)

list.sort(reverse=True)  # descending order sort
print(list)

list.reverse()          # reverse
print(list)

list.remove(1)      # remove the first occur of element
print(list)

list.pop(2)         # removes element at idx
print(list)