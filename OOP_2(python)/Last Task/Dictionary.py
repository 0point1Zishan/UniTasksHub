employee = {
    "name": "A",
    "age": 40,
    "type": {"developer" : ["ios", "android"]},
    "permanent": True,
    "salary": 30000,
    100 : (1, 2, 3),
    4.5: {5, 6, True, 7, 1}
}

#1. Print length, type, employee:
print(len(employee))
print(type(employee))
print(employee)
print()

#2. Access the key employee["type"]["developer"]
print(employee["type"]["developer"])

#3. Change the value of permanent
employee["permanent"] = False
print(employee)
print()

#4. add new key "gender" having value "male"
employee["gender"] = "male"
print(employee)
print()

#5. Remove "age" key from dictionary
del employee["age"]
print(employee)
print()

#6. Use Keys(), Values(), items()
print("Employee's keys are: ")
for x in employee.keys():
    print(x, end=" ")
print()

print("Employee's values are: ")
for x in employee.values():
    print(x, end=" ")
print()

print("Employee's items are: ")
for key, val in employee.items():
    print(f"{key} and {val}")
print()


#7. iterate the dictionary using loop:
for key, val in employee.items():
    print(f"{key}: ", end="")
    if isinstance(val, (list, tuple, set)):
        print(", ".join(map(str, val)))
    elif isinstance(val, dict):
        for i, j in val.items():
            print(f"{i} -> {j}", end="; ")
        print()
    else:
        print(val)