# Python Basic

# print Function
print("Hello World! This is a print function.")
# Newline('\n')
print("Hello World\nWelcome to Python")
# Horizontal Tab '\t'
print("Hello,\tWorld!")
# BackSlash '\\'
print("This is a backslash: \\")
# Double Quote '\"'
print("I said, \"Hello, World!\"")
# Single Quote '\''
print('It\'s a beautiful day!')


# Comments in Python
# code with comments
name = "Zishan"    # a string
print(name)


# Python Variables
# An integer
number = 100

# A floating point
cgpa = 3.71

# A string
course = "OOP-||"

print(number)
print(cgpa)
print(course)



# Python Data Types:

x = 10    # integer
x = 15.9  # float
x = True  # bool
x = b"Zishan"   # binary
x = "Zishan"    # string
x = 1j          # complex
x = ["zero", "one", "two"]    # list
X = ("Zero", "One", "two")    # tuple
x = {"Zero": 0, "[]": "list", "()": "tuple"}  # dict
x = {"Zero", "One", "two"}    # set



# Python Input-Output
a = input("Enter a string: ")
print(a)    

num = int(input())
print(num)

flo = float(input())
print(flo)

# Multiple input
a, b, c = input().split()
print(a)
print(b)
print(c)

# Multiple inputs on a Single Line specified number:
x, y, z = map(int, input().split())
print(x)
print(y)
print(z)

# Using List:
num = list(map(int, input().split()))
print(num)




# Python Operators
num1 = 10
num2 = 10

add = a + b  # addition
sub = a - b  # subtraction
mul = a * b  # multiplication
mod = a % b  # modulus
power = a ** b  # power(a^b)
div = a / b  # division
floorDiv = a // b   # floor division

print(add)
print(sub)
print(mul)
print(mod)
print(power)
print(div)
print(floorDiv)



# Logical Operator
a = True
b = False
print(a)
print(b)
print(a and b)
print(a or b)
print(not a)
print(not b)



# Bitwise Operator
a = 10
b = 4
print(a & b)  # and
print(a | b)  # or
print(~a)     # not
print(a ^ b)  # Xor
print(a >> 2) # right Shift
print(a << 2) # left shift




# Assignment Operator
a = 10
b = a
print(b)

b = 5
print(a)
print(b)

b += 5
print(b)

b //= a
print(b)

b **= a
print(b)

b -= 5
print(b)

b *= a
print(b)

b <<= a
print(b)





# Python If Else
num = int(input())
if(num < 20):
    print(num, " is smaller than 20")
elif(num > 20):
    print(num, " is greater than 20")
else:
    print(num, " is equal to 20")

# Python Loop

# For Loop:
for i in range(1, 11):
    print(i)

# While loop:
num = int(input())
i = 1
while(i < num):
    print(i)
    i += 1



    

# Python Function
def add(a, b):
    return a + b

a, b = map(int, input().split())
print(add(a, b))
