class A:
    def a_fun(self):
        print("Printing A...")

class B:
    def b_fun(self):
        print("Printing B...")

class C(A, B):
    def c_fun(self):
        print("Printing C...")

c1 = C()
print(c1.a_fun())
print(c1.b_fun())
print(c1.c_fun())