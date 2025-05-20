def add(a, b):
    return a + b

# a = int(input("Enter two number: "))
# b = int(input())
# print(add(a, b))

# def avg_cal(a, b, c):
#     avg = (a + b + c) / 3
#     print(f"Avg: {avg}")

# avg_cal(10, 8, 12)

# One Way.
def sum1(a = 5, b = 7):
    return a + b
print(sum1())

# Another way:
def sum2(a, b = 10):
    return a + b
print(sum2(5))

def sum3(a, b):
    return a + b
print(sum3(10, 15))


