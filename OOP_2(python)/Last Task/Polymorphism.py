class Department:
    def __init__(self, Name):
        self.name = Name
    def displayName(self):
        print(f"Name: {self.name}")

class Teacher(Department):
    def scheduleClass(self):
        print("Class scheduled")
    def gradeStudent(self):
        print("Student graded")
    def displayName(self):
        super().displayName()

class Author:
    def writeArticle(self):
        print("Article written")
    def publishBlog(self):
        print("Blog published")

class TeacherAuthor(Teacher, Author):
    def scheduleClass(self):
        print("TeacherAuthor schedules class")
    def gradeStudent(self):
        print("TeacherAuthor grades student")
    def writeArticle(self):
        print("TeacherAuthor writes article")
    def publishBlog(self):
        print("TeacherAuthor publishes blog")

ta = TeacherAuthor("Computer Science")
ta.scheduleClass()
ta.gradeStudent()
ta.writeArticle()
ta.publishBlog()
ta.displayName()


#1. What is Runtime Polymorphism? give a code Example:
"""
Runtime polymorphism (also known as dynamic polymorphism) is achieved through
 method overriding, where a subclass provides a specific implementation of a
 method already defined in its superclass. This allows the subclass to define
 its own behavior while sharing the same interface with the superclass.
"""

#Example:
class Animal:
    def sound(self):
        print("This is a generic animal sound.")
class Dog(Animal):
    def sound(self):
        print("Woof! Woof!")
class Cat(Animal):
    def sound(self):
        print("Meow! Meow!")

def make_sound(animal):
    animal.sound()

dog = Dog()
cat = Cat()

make_sound(dog)
make_sound(cat)
