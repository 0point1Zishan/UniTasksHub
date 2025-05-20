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

print(student)
print(student["subjects"])
print(student["subjects"]["DS"])

student["subjects"]["Math"] = 96
print(student)
print(student["subjects"]) 