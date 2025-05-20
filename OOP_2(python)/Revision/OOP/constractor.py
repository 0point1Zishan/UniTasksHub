class Dog:
    species = "Canine"  # Class attribute

    def __init__(abc, name, age):
        abc.name = name  # Instance attribute
        abc.age = age  # Instance attribute

d1 = Dog("Tom", 2)
print(d1.species)
print(d1.name)
print(d1.age)