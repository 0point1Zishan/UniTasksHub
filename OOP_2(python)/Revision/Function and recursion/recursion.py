def printValue(n):
    if(n == 0):
        return
    print(n)
    printValue(n-1)
    print("END")

printValue(10)