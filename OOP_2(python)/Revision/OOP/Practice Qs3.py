# Qs 1:
class Circle:
    def __init__(self, r):
        self.r = r

    def area(self):
        return (3.1416 * self.r ** 2)

    def  perimeter(self):
        return 2 * 3.1416 * self.r
    

c1 = Circle(2)
print(c1.area())
print(c1.perimeter())


# Qs 2:
class Employee:
    def __init__(self, role, department, salary):
        self.role = role
        self.department = department
        self.salary = salary

    def showDetails(self):
        print(f"From Department of {self.department} as a {self.role} and his basic salary {self.salary}\-.")

class Engineer(Employee):
    def __init__(self, role, department, salary, name, age):
        super().__init__(role, department, salary)
        self.name = name
        self.age = age

    def showDetails(self):
        print(f"Mr. {self.name} is {self.age} years old", end = " ")
        return super().showDetails()
    
e1 = Engineer("Software Engineer", "Development", 100000, "Zishan", 22)
e1.showDetails()


# Qs 3:

class Order:
    def __init__(self, item, price):
        self.item = item
        self.price = price
    
    def __gt__(self, a):
        return self.price > a.price
        
order1 = Order("Pizza", 200)
print(order1.item , order1.price)

order2 = Order("Burger", 300)
print(order2.item, order2.price)

if(order1 > order2):
    print("Order1's price is more than Order2")
else:
    print("Order1's price is less than Order2")