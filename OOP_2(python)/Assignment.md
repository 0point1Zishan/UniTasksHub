### 1. Dictionary:

```py
def manage_courses():

    courses = {
        "CSE101": {"Course name": "Introduction to Programming", "Credits": 3, "Instructor": "Dr. Alice"},
        "CSE102": {"Course name": "Data Structures", "Credits": 4, "Instructor": "Dr. Bob"},
        "CSE103": {"Course name": "Database Systems", "Credits": 3, "Instructor": "Dr. Carol"}
    }
    
    courses["CSE102"]["Instructor"] = "Dr. Bob Jr."
    
    courses["CSE104"] = {"Course name": "Algorithms", "Credits": 4, "Instructor": "Dr. Dave"}
    
    del courses["CSE101"]
    
    for course_code, details in courses.items():
        print(f"Course Code: {course_code}")
        for key, value in details.items():
            print(f"  {key}: {value}")
    return courses

manage_courses()
```


### 2. String:
```py
def manage_string():
    sentence = "Learning Python is fun and rewarding."
    
    substring = sentence[-26:-11]  # "Python is fun"
    print(f"Extracted substring: {substring}")
    
    modified_sentence = sentence.replace("rewarding", "exciting")
    print(f"Modified sentence: {modified_sentence}")
    
    insertion = " Keep practicing!"
    position = modified_sentence.find("exciting") + len("exciting")
    final_sentence = modified_sentence[:position] + insertion + modified_sentence[position:]
    print(f"Final sentence with insertion: {final_sentence}")
    
    capitalized_sentence = final_sentence.title()
    print(f"Capitalized sentence: {capitalized_sentence}")
    
    return capitalized_sentence

manage_string()
```

### 3. List:

```py
def manage_customers():
    customers = ["Alice", "Bob", "Charlie", "David", "Eve"]
    
    print(f"Third customer: {customers[2]}")
    
    customers[1] = "Ben"
    customers.append("Frank")
    customers.remove("David")

    customers.sort()
    print(f"Final sorted list of customers: {customers}")
    
    return customers

manage_customers()
```

### 4. Control Flow:

```py
def analyze_grades():
    grades = [85, 78, 92, 45, 33, 67, 88, 41]
    
    def categorize_grade(grade):
        if grade > 80:
            return 'A'
        elif 60 <= grade <= 80:
            return 'B'
        elif 40 <= grade <= 60:
            return 'C'
        else:
            return 'F'

    print("Grade Categories:")
    for grade in grades:
        print(f"Score: {grade} - Grade: {categorize_grade(grade)}")
    
    boosted_grades = list(map(lambda g: g * 1.05, grades))
    print(f"Boosted Grades: {boosted_grades}")
    
    above_90 = list(filter(lambda g: g > 90, boosted_grades))
    print(f"Boosted Grades Above 90: {above_90}")

analyze_grades()
```

### 5. Tuple & Set:
```py
def manage_library():
    books = (
        ("To Kill a Mockingbird", "Harper Lee", 1960),
        ("1984", "George Orwell", 1949),
        ("The Great Gatsby", "F. Scott Fitzgerald", 1925)
    )
    
    tags = {"classic", "dystopian", "novel", "literature"}
    
    print(f"Second book's author: {books[1][1]}")
    
    books = books + (("Brave New World", "Aldous Huxley", 1932),)
    
    title, author, year = books[2]
    print(f"Unpacked third book: Title={title}, Author={author}, Year={year}")
    
    for book in books:
        print(f"Book Title: {book[0]}")
    
    tags.add("sci-fi")
    print(f"Updated tags: {tags}")
    
    tags.discard("novel")
    print(f"Tags after removal: {tags}")

manage_library()
```