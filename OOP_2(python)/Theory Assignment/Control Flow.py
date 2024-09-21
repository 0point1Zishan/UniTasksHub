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

    def boost_grades(grades):
        return list(map(lambda g: g * 1.05, grades))

    print("Grade Categories:")
    for grade in grades:
        print(f"Score: {grade} - Grade: {categorize_grade(grade)}")
    
    boosted_grades = boost_grades(grades)
    print(f"Boosted Grades: {boosted_grades}")
    
    above_90 = list(filter(lambda g: g > 90, boosted_grades))
    print(f"Boosted Grades Above 90: {above_90}")

analyze_grades()