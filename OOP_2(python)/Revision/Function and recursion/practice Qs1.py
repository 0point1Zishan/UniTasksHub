# Qs1:
def length(lst):
    return len(lst)

# Qs2:
def pnt(lst):
    for i in lst:
        print(i, end=" ")
    print()

a = [1, 2, 3, 4, 5]
print(length(a))
pnt(a)

# Qs 3:
def factorial(n):
    fact = 1
    for i in range(1, n+1):
        fact *= i
    return fact

print(factorial(5))


# Qs 4:
def convetDollarToTaka(dollar):
    return dollar * 121.66
print(convetDollarToTaka(5))

# Homework:
def evenOrODD(num):
    if num & 1:
        print("ODD")
    else:
        print("Even")
        
evenOrODD(5)
evenOrODD(2)
evenOrODD(10)