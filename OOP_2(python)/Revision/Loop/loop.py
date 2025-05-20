
# While Loop:
i = 0
while i < 5:
    print("Hello")
    i += 1
print("")

count = 5
while count >= 0:
    print(count)
    count -= 1


# For loop:
print()
for i in range(10):
    print(i, end=" ")
print()

print()
for i in range(5, 10):
    print(i, end=" ")
print()

num = (1, 2, 3, 4, 5)
lst = ['a', 'b', 'c', 1, 2, 10, "Abc", True, 15.5]
for i in num:
    print(i, end=" ")
else:
    print

for i in lst:
    print(i, end=" ")
print()

# After loop with else will work only if there no break statement in loop, else will work.
print("Hello for loop.")
for i in range(11):
    print(i)
    if(i == 5):
        break
else:
    print("End")

# Range:
for i in range(11):
    print(i, end=" ")
print()

for i in range(10, 21):
    print(i, end=" ")
print()

for i in range(20, 50, 5):
    print(i, end=" ")
print("\n")



## Pas:
for i in range(5):
    pass
print("Some work should be done.")