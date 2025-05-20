student = {
    "name" : "Md. Zishan",
    "subjects": {
        "PPS": 95,
        "DS": 98,
        "OOP": 97,
        "Algo": 99
    },
    "grade": "A+"
}

print(student.keys())

print(student.values())

print(student.items())

print(len(student))
print(len(student["subjects"]))

pair = list(student.items())
print(pair[1])


print(student["name"])
print(student.get("name"))

# print(student["name2"])  #error
print(student.get("name2"))  # no error -> none

new_dic = {"name": "Murad", "age": 22}
student.update(new_dic)
print(student)