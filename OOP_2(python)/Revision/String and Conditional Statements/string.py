str1 = "1st string"
str2 = '2nd String'
str3 = '''3th String'''
str4 = """4th Sting"""

print(str1, str2, str3, str4)

str = 'This is "Python Language" .'
str1 = "This is a 'python Language' ."
print(str)
print(str1)


# New Line by \n
print("\tThis is a first line and\n This is a 2nd line.")


# Concatenation:
str1 = "Hello"
str2 = "World."

str = str1 + " " + str2
print('\n',str)

# Length function:
print(len(str))


# Indexing and negative indexing:
str = "Md. Zishan"
print(str[0], str[1])

print(str[-1])

str += "@"
# str[-1] = '#'     # We can not manipulate them


# slicing:
print(str[0 : 3])
# Or:
print(str[: 3])

print(str[4 : len(str)])
# Or:
print(str[4: ])
# Or:
print(str[4: -1])


# String functions:
# Note: after apply those function the original function will not be change.
print("\n\n")
str = "i am a coder."
print(str.endswith("er.")) # return true/false

print(str.capitalize())
print(str)

print(str.replace("a", "o"))   # change all the occur

print(str.find("word"))   # return 1st index of 1st occurrence(if not present return -1)

print(str.count("am")) # number of occur