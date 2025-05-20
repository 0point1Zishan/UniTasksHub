f = open("demo.txt", "rt")
data = f.read()
print(data)
print(type(data))
f.close()

# If we want to print only fist n number of character, then:
f = open("demo.txt", "rt")
data1 = f.read(3)
print(data1)
f.close

# For read line by line, then:
f = open("demo.txt", "rt")
while True:
    line = f.readline()
    if(len(line) == 0):
        break
    print(line)




## Write in file:
f = open("demo.txt", "w")                   # "W" used for overwrite
f.write("I am learning python language.")
f.close()

f = open("demo.txt", "a")
f.write("\nI am already completed 7 lecture. There are remain only 2 lecture.")
# For append next line we need to write \n
f.close()