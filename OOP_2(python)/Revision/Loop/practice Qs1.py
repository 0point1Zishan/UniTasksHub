# Qs 1:
i = 1
while i <= 100:
    print(i, end=" ")
    i += 1
print("\n")

# Qs 2:
i = 100
while(i >= 1):
    print(i, end=" ")
    i -= 1
print("\n")

# Qs 3:
n = int(input("Enter a number: "))
i = 1
while i <= 10:
    print(f"{n} * {i} = {n * i}")
    i += 1
print("\n")

#Qs. 4:
lst = [1, 4, 9, 16, 25, 36, 49, 64, 81, 100]
i = 0
while i < len(lst):
    print(lst[i], end=" ")
    i += 1
print("\n")


# Qs. 5:
tup = tuple(lst)
num = int(input("Enter a number to search: "))

# search using builtin function:
print(f"Tuple is: {tup}")
if num in tup: 
    print(f"{num} is present in index {tup.index(num)}")
else:
    print(f"{num} is not present in this tuple.")


# Search using loop:
i = 0
while i < len(tup):
    if(tup[i] == num):
        print(f"{num} is present in index {i}")
        break
    i += 1

print(f"{num} is not present in this tuple.")