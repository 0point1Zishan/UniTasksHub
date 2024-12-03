def isPrime(n):
    if(n == 1):
         return False
    for i in range(2, int(n**.5)+1):
        if(n % i == 0):
            return False
    return True

n = int(input("Enter a number: "))
if(isPrime(n)):
    print(f"{n} is a prime number.")
else:
    print(f"{n} is not a prime number")