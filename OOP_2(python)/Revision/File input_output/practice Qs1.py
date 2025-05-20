# # Practice Qs -1:
# f = open("practice_Qs1.txt", "w")
# f.write("Hi everyone\nwe are learning File I/O\nusing Java.\nI like programming in Java.")
# f.close()

# # Practice Qs -2:
# with open("practice_Qs1.txt", "r") as f:
#     data = f.read()

# data = data.replace("Java", "Python")
# with open("practice_Qs1.txt", "w") as f:
#     f.write(data)

# # Practice Qs - 3:
# with open("practice_Qs1.txt", "r") as f:
#     data = f.read()
#     if("xlearning" in data):
#         print("Exists")
#     else:
#         print("Not exists")


# practice Qs - 4:
def checkWord(word, file):
    i = 1
    with open(file, "r") as f:
        while True:
            line = f.readline()
            if len(line) == 0:
                return -1
            if word in line:
                return i
            i += 1

print(checkWord("learning", "practice_Qs1.txt"))

# Practice Qs - 5:
with open("practice_Qs1.txt", "r") as f:
    data = f.read()
    data = data.split(',')

countEven = 0
for i in data:
    i = int(i)
    if (i % 2 == 0):
        countEven += 1

print(f"Number of even numbers: {countEven}")
