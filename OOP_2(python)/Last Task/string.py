a = "hello"
b = "b2b2b2  "
c = " 3g3g.  "

#1. declare a new variable d and concate(a, b, c):
d = a + b + c
print(d)

#2. find the length of d and print d[:-3]
print(len(d[:-3]))

#3. check "a2" is present in d:
print("YES" if "a2" in d else "NO")

#4. 

#upper
print(d.upper())

#lower:
print(d.lower())

#title:
print(d.title())

#strip()
print(d.strip())

#isDigit:
print(d.isdigit())

#find("3g")
print(d.find("3g"))

#capatalize:
print(d.capitalize())

#isalnum():
print(d.isalnum())

#count("b2")
print(d.count("b2"))

#split()
print(d.split())

#swapcase:
print(d.swapcase())

#lstrip():
print(d.lstrip())

#replace "hello" with "python":
print(d.replace("hello", "python"))