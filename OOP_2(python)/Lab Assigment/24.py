def find_hcf(x, y):
    while y:
        x, y = y, x % y
    return x

num1 = 54
num2 = 24
print(f"HCF of {num1} and {num2} is {find_hcf(num1, num2)}")
