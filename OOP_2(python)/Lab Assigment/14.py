lower = 10
upper = 50
print(f"Prime numbers between {lower} and {upper} are:")
for num in range(lower, upper + 1):
    if num > 1:
        for i in range(2, int(num/2) + 1):
            if num % i == 0:
                break
        else:
            print(num)
