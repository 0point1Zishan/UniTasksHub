a = (1, 3, 5, 7, 4)

# a. Find the sum of all odd number in a
sum = 0
for x in a:
    if(x & 1):
        sum += x
print(f"Sum of all odd numbers in a: {sum}")

#b. Find the sum of all odd index element in a:
sum = 0
for i in range(len(a)):
    if(i & 1):
        sum += a[i]
print(f"Sum of all odd index elements in a: {sum}")

#c. Count the number of odd and even number separately:
odd, even = 0, 0
for x in a:
    if(x & 1):
        odd += 1
    else:
        even += 1
print(f"In a odd numbers are: {odd} and even numbers are: {even}")


#d. Extend the tuple with (2, 4, 6)
a = a + (2, 4, 6)
print(a)

#e. add a new item in index 2(400):
a = a[:2] + (400, ) + a[2:]
print(a)

#f. Remove the last element:
a = a[:-1]
print(a)

#g. Perform slicing[-4: -1]
print(a[-4:-1])

#h. Print the tuple using loop and use continue if get 5:
for x in a:
    if(x == 5):
        continue
    print(x, end=" ")
print()

