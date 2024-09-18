a = "hello"
b = "b2b2b2"
c = "3939     "

d = a + " " + b + " " + c
print(d)

# Length of d:
print(len(d))

# Printing using range:
for i in d[:-3]:
    print(i, end="")
print()

# Upper:
upperFun = d.upper()
print(upperFun)

# Lower:
lowerFun = d.lower()
print(lowerFun)

# Title:
titleD = d.title()
print(titleD)

# Strip:
StripD = d.strip()
print(StripD)
print(len(StripD))  # Length after strip

# isDigit:
DisDigit = d.replace(" ", "").isdigit()  # Remove spaces before checking
print(DisDigit)

# find:
a = d.find("39")
print(a)

# Capitalize:
a = d.capitalize()
print(a)

# isalnum:
a = d.replace(" ", "").isalnum()  # Remove spaces before checking
print(a)

# Count:
count = d.count("b2")
print(count)

# Split:
a = d.split()
print(a)

# Replace:
a = d.replace("hello", "python")
print(a)

# SwapCase:
a = d.swapcase()
print(a)

# Check:
a = d.find("a2")
print(a)

# Check by in function:
if "a2" in d:
    print("a2 is present in D")
else:
    print("Not present")
print()

# Lambda function:
try:
    a, b, c = map(int, input("Enter three integers separated by space: ").split())
    x = lambda a, b, c: a * b + c
    print(x(a, b, c))
except ValueError:
    print("Invalid input. Please enter three integers.")

# List comprehension:
fruits = ["apple", "banana", "cherry", "mango"]
list_a = [x for x in fruits if 'a' in x]
print(list_a)

a = [2, 7, 10, 15, 8]
p = [a[i] for i in range(len(a)) if i & 1]  # Using list comprehension for odd indices
print(p)

# Reverse a string
text = "Hello"
b = text[::-1]
print(b)

x = lambda a: len(a)
ans = map(x, ("apple", "banana", "kola"))
print(list(ans))

b = lambda a, b: a ** b
list_a = map(lambda x: b(*x), [(2, 3), (4, 5), (6, 2)])
print(list(list_a))
print()

a = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
for row in a:
    for col in row:
        print(col, end=" ")
    print()