class A:
    def a_fun(self):
        print("Printing A...")

class B(A):
    def b_fun(self):
        print("Printing B...")


b1 = B()
print(b1.a_fun())
print(b1.b_fun())