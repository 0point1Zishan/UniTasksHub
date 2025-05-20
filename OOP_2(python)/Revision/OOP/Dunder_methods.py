class Complex:
    def __init__(self, real, img):
        self.real = real
        self.img = img

    def showNumber(self):
        print(self.real,"i + ", self.img, "j")

    def __add__(self, num):
        new_Real = self.real + num.real
        new_Img = self.img + num.img
        return Complex(new_Real, new_Img) 
    
    def __sub__(self, num):
        new_Real = self.real - num.real
        new_Img = self.img - num.img
        return Complex(new_Real, new_Img) 
    
    def __mul__(self, num):
        new_Real = self.real * num.real
        new_Img = self.img * num.img
        return Complex(new_Real, new_Img) 
    
    def __floordiv__(self, num):
        new_Real = self.real / num.real
        new_Img = self.img / num.img
        return Complex(new_Real, new_Img)

    def __truediv__(self, num):
        new_Real = self.real / num.real
        new_Img = self.img / num.img
        return Complex(new_Real, new_Img)
    
    def __mod__(self, num):
        new_Real = self.real % num.real
        new_Img = self.img % num.img
        return Complex(new_Real, new_Img)
    
    
num1 = Complex(1, 3)
num1.showNumber()

num2 = Complex(4, 8)
num2.showNumber()

num3 = num1 + num2
num3.showNumber()

num4 = num1 - num2
num4.showNumber()

num5 = num1 * num2
num5.showNumber()

num6 = num1 // num2
num6.showNumber()

num8 = num1 / num2
num8.showNumber()

num7 = num2 % num1
num7.showNumber()