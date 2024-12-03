class Vehicle:
    def __init__(self, color):
        self.color = color
    def vehicleInfo(self):
        print(f"Vehicle's color: {self.color}")

class Taxi(Vehicle):
    def __init__(self, color, model, capacity, variant):
        super().__init__(color)
        self.__model = model
        self.__capacity = capacity
        self.__variant = variant


    def getModel(self):
        return self.__model
    def getCapacity(self):
        return self.__capacity
    def getVariant(self):
        return self.__variant
    def setModel(self, model):
        self.__model = model
    def setCapacity(self, capacity):
        self.__capacity = capacity
    def setVariant(self, variant):
        self.__variant = variant

    def vehicleInfo(self):
        print(f"Taxi Color: {self.color}, model: {self.__model}, capacity: {self.__capacity} and variant: {self.__variant}")
        return super().vehicleInfo()

t1 = Taxi("Yellow", "Sedan", 4, "Standard")
t2 = Taxi("Black", "SUV", 7, "Luxury")

t1.vehicleInfo()
print()
t2.vehicleInfo()