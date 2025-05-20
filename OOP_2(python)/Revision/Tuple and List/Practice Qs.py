# Qs 1:
movies = []
print("Enter 3 movies name: ")
for i in range(3):
    movies.append(input())

print(movies)

# Qs 2:
list1 = [1, 2, 3, 2, 1]
list2 = list1.copy()

list2.reverse()
if(list1 == list2): 
    print("Palindrome")
else:
    print("Not a palindrome")

# Or:
s = 0
e = len(list1) - 1
isPalindrome = True
while(s <= e):
    if(list1[s] != list1[e]):
        isPalindrome = False
        break
    s += 1
    e -= 1

if(isPalindrome):
    print("Palindrome")
else:
    print("Not a palindrome")    


# Qs. 3:
grade = ('C', 'D', 'A', 'A', 'B', 'B', 'A')
print(grade.count('A'))

listGrade = []
for i in grade:
    listGrade.append(i)
    
listGrade.sort()
print(listGrade)