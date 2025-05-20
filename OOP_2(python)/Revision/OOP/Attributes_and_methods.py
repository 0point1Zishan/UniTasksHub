class Person:
    __name = "Anno"
    age = 10

    def __hello(self):
        print("Hello Person!")

    def welcome(self):
        self.__hello()


p1 = Person()
# print(p1.__name)        # Error, generally python can access private attributes
print(p1.age)
# print(p1.hello())

print(p1.welcome())