# Qs. 1:
list = [1, 4, 9, 16, 25, 36, 49, 64, 81, 100]
for i in list:
    print(i, end=" ")
print("\n")

# Qs. 2:
num = int(input("Enter a number: "))
for i in range(0, len(list)):
    if(list[i] == num):
        print(f"{num} is found at index {i}.")
        break
else:
    print(f"{num} is not present in this list")

# Qs. 3:
for i in range(101):
    print(i, end=" ")
print("\n")

# Qs. 4:
for i in range(100, 0, -1):
    print(i, end=" ")
print("\n")

# Qs. 5:
num = int(input("Enter a number: "))
for i in range(1, 11):
    print(f"{num} * {i} = {num * i}")
print("\n")


# Qs. 6:
num = int(input("Enter a number: "))
sum = 0
i = 1
while i <= num:
    sum += i
    i += 1

print(f"Total sum = {sum}.")

# Qs. 7:
num = int(input("Enter a number: "))
fact = 1
for i in range(1, num+1):
    fact *= i
print(f"Factorial = {fact}.")