class Car:
    def __init__(self, type):
        self.type = type
    
    @staticmethod
    def start():
        print("Car started...")
    def stop():
        print("Car stopped...")

    
class BMW(Car):
    def __init__(self, name, type):
        super().__init__(type)
        self.name = name
        super().start()


car1 = BMW("Sedans", "electrics")
print(car1.type)