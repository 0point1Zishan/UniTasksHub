#1.
class Product:
    def __init__(self, name, price):
        self.name = name
        self.price = price
    def display_detail(self):
        print(f"{self.name}'s price is {self.price}.")

class ElectronicProduct(Product):
    def __init__(self, name, price, warranty):
        super().__init__(name, price)   # use super() and set name, price
        self.warranty = warranty
    def display_detail(self):
        print(f"{self.name}'s price is {self.price} and warranty {self.warranty} years.")

p1 = Product("Laptop", 1000)
p1.display_detail()

eP1 = ElectronicProduct("Phone", 8000, 2)
eP1.display_detail()
print()


#2.
class Shape:
    def __init__(self, name):
        self._name = name
    def get_name(self):
        return self._name

    def __display(self):
        pass
    def displayInfo(self):
        print(f"Shape name is: {self._name}.")

class Rectangle(Shape):
    def __init__(self, name, length, width):
        super().__init__(name)
        self.__length = length
        self.__width = width
    
    def area(self):
        print(f"Rectangle's area is: {self.__length * self.__width}")
    def perimeter(self):
        print(f"Rectangle's perimeter is: {2 * (self.__length + self.__width)}")

S1 = Shape("General")
S1.displayInfo()

r1 = Rectangle("Rectangle", 10, 5)
r1.area()
r1.perimeter()