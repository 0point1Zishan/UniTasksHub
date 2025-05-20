class Student:
    def __init__(self, name, mark):
        self.name = name
        self.mark = mark
    
    def average(self):
        sum = 0
        for i in self.mark:
            sum += i
        print(f"Average mark of {self.name} = {sum / len(self.mark)}.")
    
stu1 = Student("Zishan", [99, 98, 100])
stu1.average()