def fibonacci(n):
    if n <= 1:
        return n
    else:
        return fibonacci(n - 1) + fibonacci(n - 2)

n_terms = 10
if n_terms <= 0:
    print("Please enter a positive integer")
else:
    for i in range(n_terms):
        print(fibonacci(i))
