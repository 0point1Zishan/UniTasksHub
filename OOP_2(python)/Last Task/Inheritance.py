class Person:
    def __init__(self, firstName, lastName):
        self._firstName = firstName
        self._lastName = lastName
    def display(self):
        print(f"Name: {self._firstName} {self._lastName}.")

class Admin(Person):
    def __init__(self, firstName, lastName, joiningYear):
        super().__init__(firstName, lastName)
        self.__joiningYear = joiningYear
    def display(self):
        super().display()
        print(f"Admin Joining Year: {self.__joiningYear}.")

class Teacher(Person):
    def __init__(self, firstName, lastName, joiningYear):
        super().__init__(firstName, lastName)
        self.__joiningYear = joiningYear
    def display(self):
        super().display()
        print(f"Teacher Joining Year: {self.__joiningYear}.")

class Student(Person):
    def __init__(self, firstName, lastName, graduationYear):
        super().__init__(firstName, lastName)
        self.__graduationYear = graduationYear
    def display(self):
        super().display()
        print(f"Graduation Year: {self.__graduationYear}.")

class Alumni(Student):
    def __init__(self, firstName, lastName, graduationYear, passingYear):
        super().__init__(firstName, lastName, graduationYear)
        self.__passingYear = passingYear
    def display(self):
        super().display()
        print(f"Passing Year: {self.__passingYear}.")

class CurrentStudent(Student):
    def __init__(self, firstName, lastName, graduationYear, currentSemester):
        super().__init__(firstName, lastName, graduationYear)
        self.__currentSemester = currentSemester
    def display(self):
        super().display()
        print(f"Current Semester: {self.__currentSemester}.")

class Employee(Person):
    def __init__(self, firstName, lastName, adminJoiningYear, teacherJoiningYear, id):
        Person.__init__(self, firstName, lastName)
        self.__adminJoiningYear = adminJoiningYear
        self.__teacherJoiningYear = teacherJoiningYear
        self.__id = id
    def display(self):
        super().display()
        print(f"Admin Joining Year: {self.__adminJoiningYear}.")
        print(f"Teacher Joining Year: {self.__teacherJoiningYear}.")
        print(f"ID: {self.__id}.")

p1 = Person("John", "Doe")
s1 = Student("Jane", "Doe", 2022)
alu1 = Alumni("Alice", "Smith", 2018, 2022)
cur_stu = CurrentStudent("Bob", "Brown", 2023, "Second Semester")
t1 = Teacher("Charlie", "Black", 2015)
ad1 = Admin("Dave", "White", 2010)
emp = Employee("Eve", "Johnson", 2010, 2015, "E12345")

p1.display()
s1.display()
alu1.display()
cur_stu.display()
t1.display()
ad1.display()
emp.display()
