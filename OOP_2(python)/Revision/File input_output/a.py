# # ## READ file:
# f = open("file.txt", "r")
# data = f.read()
# print(data)
# print(type(data))
# f.close() 


# # Number of character read
# f = open("file.txt", "r")
# data = f.read(5)
# print(data)
# print(type(data))
# f.close() 

# # read line by line
# f = open("file.txt", "r")
# data = f.readline()
# print(data)

# data = f.readline()
# print(data)


# # Write to a file:
# f = open("file.txt", "w")       # for overwrite file
# f.write("I am learning python file from Apna College")
# f.close()


# f = open("file.txt", "a")        # append at the end txt
# f.write("\nI already learned other basic concept")
# f.close()


# # Overwrite from starting of a file
# f = open("file.txt", "r+")
# f.write("He is")
# print(f.read())         # read will be after overwrite pointer.
# f.close()

# f = open("file.txt", "w+")      # delete all txt from this file
# print(f.read())
# f.close()

# f = open("new_file.txt", "a+")   # opens the file for both reading and writing, positioning the file pointer at the end for writing in existing files and creating a new, empty file if it doesn't exist.
# print(f.read())
# f.close()

# f = open("file.txt", "a+")
# print(f.read())

# f.write("\nNow, I am learning python language.")
# f.close()


## With syntax:
with open("file.txt", "r") as f:
    data = f.read()
    print(data)

with open("file.txt", "w") as f:
    f.write("First, deleted all previous text and added new text in this file.")
