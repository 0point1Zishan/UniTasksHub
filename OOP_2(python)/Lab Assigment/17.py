n_terms = 10
n1, n2 = 0, 1
count = 0
if n_terms <= 0:
    print("Please enter a positive integer")
elif n_terms == 1:
    print(f"Fibonacci sequence up to {n_terms}: {n1}")
else:
    print("Fibonacci sequence:")
    while count < n_terms:
        print(n1)
        nth = n1 + n2
        n1 = n2
        n2 = nth
        count += 1
