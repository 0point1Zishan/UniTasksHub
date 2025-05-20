# if-elif-else(Syntax)
mark = int(input("Enter you mark: "))
if(mark >= 90):
    print("Grade: A")
elif(mark >= 80 and mark < 90):
    print("Grade: B")
elif(mark >= 70 and mark < 80):
    print("Grade: C")
else:
    print("Grade: D")