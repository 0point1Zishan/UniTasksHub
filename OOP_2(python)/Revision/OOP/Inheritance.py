class Car:
    @staticmethod
    def start():
        print("Car started...")
    
    @staticmethod
    def stop():
        print("Car stopped...")

class BMW(Car):
    def __init__(self, name):
        self.name = name


car1 = BMW("Sedans")
car2 = BMW("SUVs")

print(car1.start())