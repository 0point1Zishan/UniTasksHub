import numpy as np

# Encapsulation and Abstraction
class User:
    def __init__(self, id, name, email, role):
        self.__id = id
        self.__name = name
        self.__email = email
        self.__role = role

    def login(self, password):
        if password == "password123":
            print(f"{self.__name} has logged in.")
            return True
        else:
            print("Invalid password.")
            return False

    def view_dashboard(self):
        print(f"{self.__name}'s Dashboard")

    # Getter and Setter methods for encapsulated attributes
    def get_id(self):
        return self.__id

    def set_id(self, id):
        self.__id = id

class Employee(User):
    def __init__(self, id, name, email, role, tasks, leave_requests, performance_reviews):
        super().__init__(id, name, email, role)
        self.tasks = tasks
        self.leave_requests = leave_requests
        self.performance_reviews = performance_reviews

    def request_leave(self, leave_request):
        self.leave_requests = leave_request
        print(f"{self.get_id()} requested leave: {leave_request}")

    def update_task_status(self, task, status):
        task.update_status(status)
        print(f"{self.get_id()} updated task {task.title} status to {status}")

class Manager(User):
    def __init__(self, id, name, email, role, team):
        super().__init__(id, name, email, role)
        self.team = team

    def add_team_member(self, member):
        self.team.append(member)
        print(f"Added {member} to team")

class Designer(Employee):
    def work(self):
        print("Designing UI...")

class Developer(Employee):
    def work(self):
        print("Writing code...")

class Finance(User):
    def __init__(self, id, name, email, role):
        super().__init__(id, name, email, role)

    def pay_salary(self, net_salary):
        print(f"Salary paid: {(net_salary.get_basic_salary() + net_salary.get_overtime()) - net_salary.get_absentee()}")

class Net_Salary:
    def __init__(self, basic_salary, overtime, absentee):
        self.__basic_salary = basic_salary
        self.__overtime = overtime
        self.__absentee = absentee

    def get_basic_salary(self):
        return self.__basic_salary

    def get_overtime(self):
        return self.__overtime

    def get_absentee(self):
        return self.__absentee

    def view_salary(self):
        print(f"Your Total Salary: {self.__basic_salary + self.__overtime - self.__absentee}")

    def withdraw_salary(self):
        print(f"Withdrawing salary:{self.__basic_salary + self.__overtime - self.__absentee}")

class Project:
    def __init__(self, title, deadline):
        self.title = title
        self.deadline = deadline
        #set
        self.team_members = set()

    def start_project(self):
        print(f"Project {self.title} started.")

    def complete_project(self):
        print(f"Project {self.title} completed.")

    def add_team_member(self, member):
        self.team_members.add(member)
        print(f"Added {member} to project {self.title}")

class Task:
    def __init__(self, task_id, title, description, assigned_to, status):
        self.task_id = task_id
        self.title = title
        self.description = description
        self.assigned_to = assigned_to
        self.status = status

    def update_status(self, status):
        self.status = status
        print(f"Task {self.task_id} status updated to {self.status}")

    def add_comment(self, comment):
        print(f"Comment added to task {self.task_id}: {comment}")

class PerformanceReview:
    def __init__(self, review_id, metrics, comments, score):
        self.review_id = review_id
        self.metrics = metrics
        self.comments = comments
        self.score = score

    def generate_report(self):
        print(f"Performance report for review {self.review_id}: Metrics: {self.metrics}, Score: {self.score}")

    def add_feedback(self, feedback):
        print(f"Feedback added to review {self.review_id}: {feedback}")

class ProductManager(Manager):
    def assign_task(self, task):
        task.assigned_to = self.team
        print(f"Task {task.title} assigned to team")

    def evaluate_performance(self, review):
        review.generate_report()

# Dictionary
projects = {
    "project1": Project("Website Redesign", "2024-12-31"),
    "project2": Project("Mobile App Development", "2025-06-30")
}

# Lambda functions
calculate_overtime_payment = lambda hours, rate: hours * rate
calculate_absentee_penalty = lambda days, rate: days * rate

# Numpy
salaries = np.array([3000, 3200, 2800, 2900])
mean_salary = np.mean(salaries)
std_salary = np.std(salaries)

# String
developer = "Full Stack Developer"
developer_upper = developer.upper()
developer_lower = developer.lower()
developer_replace = developer.replace("Full", "Junior")

# recursion
def factorial(n):
    if n == 0:
        return 1
    else:
        return n * factorial(n-1)

factorial_5 = factorial(5)

# Slicing
team_members = ["Zishan", "Fahim", "Faria", "Murad"]
team_members_slice = team_members[1:3]

# Common Numpy functions
array = np.array([1, 2, 3, 4, 5])
sum_array = np.sum(array)
product_array = np.prod(array)

# Common string functions
text = "Hello, World!"
len_text = len(text)
sub_text = text[0:6]

def main_menu():
    print("\nMain Menu")
    print("1. Login")
    print("2. Exit")
    choice = input("Enter your choice: ")
    return choice

def role_menu(role):
    if role == "Developer":
        print("\nDeveloper Menu")
        print("1. Work on a project")
        print("2. View/Withdraw salary")
        print("3. View additional information")
        print("4. View Dashboard")
        print("5. Request leave")
        print("6. Exit")
    elif role == "Designer":
        print("\nDesigner Menu")
        print("1. Work on a project")
        print("2. View/Withdraw salary")
        print("3. View additional information")
        print("4. View Dashboard")
        print("5. Request leave")
        print("6. Exit")
    elif role == "Manager":
        print("\nManager Menu")
        print("1. Manage a team")
        print("2. View/Withdraw salary")
        print("3. View additional information")
        print("4. View Dashboard")
        print("5. Exit")
    elif role == "ProductManager":
        print("\nProduct Manager Menu")
        print("3. View additional information")
        print("4. View Dashboard")
        print("5. Exit")
    elif role == "Finance":
        print("\nFinance Menu")
        print("1. Pay salary")
        print("2. View/Withdraw salary")
        print("3. View additional information")
        print("4. View Dashboard")
        print("5. Exit")
    choice = input("Enter your choice: ")
    return choice

def handle_login():
    try:
        user_id = int(input("Enter ID: "))
        user_name = input("Enter Name: ")
        user_email = input("Enter Email: ")
        user_role = input("Enter Role (Finance, Manager, ProductManager, Developer, Designer): ")
        user_password = input("Enter Password: ")

        roles = ["Finance", "Manager", "ProductManager", "Developer", "Designer"]
        if user_role not in roles:
            print("Invalid role")
            return None

        if user_role == "Developer":
            employee = Developer(user_id, user_name, user_email, user_role, [], [], [])
        elif user_role == "Designer":
            employee = Designer(user_id, user_name, user_email, user_role, [], [], [])
        elif user_role == "Manager":
            employee = Manager(user_id, user_name, user_email, user_role, [])
        elif user_role == "ProductManager":
            employee = ProductManager(user_id, user_name, user_email, user_role, [])
        elif user_role == "Finance":
            employee = Finance(user_id, user_name, user_email, user_role)
        else:
            print("Invalid role")
            return None

        if not employee.login(user_password):
            return None
        else:
            return employee
    except Exception as e:
        print(f"An error occurred during login: {e}")
        return None

def handle_role_action(employee):
    try:
        role = type(employee).__name__
        while True:
            choice = role_menu(role)
            
            if choice == "1":
                if role in ["Developer", "Designer"]:
                    project_name = input("Enter the project you are working on: ").strip().lower()
                    if project_name in [name.lower() for name in projects.keys()]:
                        project = projects[[name for name in projects if name.lower() == project_name][0]]
                        task_id = input("Enter Task ID: ")
                        task_title = input("Enter Task Title: ")
                        task_desc = input("Enter Task Description: ")
                        task_status = input("Enter Task Status: ")
                        task = Task(task_id, task_title, task_desc, employee.get_id(), "In Progress")
                        employee.update_task_status(task, task_status)
                        project.add_team_member(employee.get_id())
                    else:
                        print("Invalid project name.")
                elif role == "Manager":
                    team_member = input("Enter team member to add: ")
                    employee.add_team_member(team_member)
                elif role == "ProductManager":
                    project_name = input("Enter the project you are managing: ").strip().lower()
                    if project_name in [name.lower() for name in projects.keys()]:
                        project = projects[[name for name in projects if name.lower() == project_name][0]]
                        project.start_project()
                        project.complete_project()
                    else:
                        print("Invalid project name.")
                elif role == "Finance":
                    net_salary_basic = float(input("Enter Basic Salary: "))
                    net_salary_overtime = float(input("Enter Overtime Amount: "))
                    net_salary_absentee = float(input("Enter Absentee Amount: "))
                    net_salary = Net_Salary(net_salary_basic, net_salary_overtime, net_salary_absentee)
                    net_salary.view_salary()
                    employee.pay_salary(net_salary)

            elif choice == "2":
                if role in ["Developer", "Designer", "Manager", "ProductManager", "Finance"]:
                    net_salary_basic = float(input("Enter Basic Salary: "))
                    net_salary_overtime = float(input("Enter Overtime Amount: "))
                    net_salary_absentee = float(input("Enter Absentee Amount: "))
                    net_salary = Net_Salary(net_salary_basic, net_salary_overtime, net_salary_absentee)
                    view_or_withdraw = input("Do you want to view or withdraw salary? (view/withdraw): ")
                    if view_or_withdraw == "view":
                        net_salary.view_salary()
                    elif view_or_withdraw == "withdraw":
                        net_salary.withdraw_salary()
                    else:
                        print("Invalid option")
            elif choice == "3":
                show_additional_info = input("Do you want to see additional information like mean salary, string operations, etc.? (yes/no): ")
                if show_additional_info.lower() == "yes":
                    print(f"Mean Salary: {mean_salary}, Standard Deviation: {std_salary}")
                    print(developer_upper)
                    print(developer_lower)
                    print(developer_replace)
                    print(f"Factorial of 5 is {factorial_5}")
                    print(team_members_slice)
                    print(f"Sum: {sum_array}, Product_ {product_array}")
                    print(f"Length: {len_text}, Substring: {sub_text}")
            elif choice == "4":
                employee.view_dashboard()
            elif choice == "5":
                if role in ["Developer", "Designer"]:
                    leave_request = input("Enter leave request details: ")
                    employee.request_leave(leave_request)
                else:
                    print("Exiting to main menu...")
                    break
            elif choice == "6":
                print("Exiting to main menu...")
                break
            else:
                print("Invalid choice. Please try again.")
    except Exception as e:
        print(f"An error occurred while handling role actions: {e}")

if __name__ == "__main__":
    while True:
        try:
            choice = main_menu()
            if choice == "1":
                employee = handle_login()
                if employee:
                    handle_role_action(employee)
            elif choice == "2":
                print("Goodbye!")
                break
            else:
                print("Invalid choice. Please try again.")
        except Exception as e:
            print(f"An error occurred in the main loop: {e}")