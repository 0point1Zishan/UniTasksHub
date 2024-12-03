sol = lambda a, b: (a * a) + (b * b) + (2 * a * b)

a, b = map(int, input("Enter two numbers: ").split())
print(f"The Square of ({a} + {b}) is equal to : {sol(a, b)}.")