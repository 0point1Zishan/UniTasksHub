import json
import os
import numpy as np

# Employee data
employee = {}
departments = {'Manager', 'Developer', 'Designer', 'Finance'}
employee_data = "employee_data.json"

# Predefined salaries by department
department_salaries = {"Manager": 80000, "Developer": 60000, "Designer": 50000, "Finance": 55000}

# Salary payments and leave requests
salary_payments = []
leave_requests = []

# Projects with assigned employees
projects = {"Project A": [], "Project B": []}

# Load employee data from file if it exists
if os.path.exists(employee_data):
    with open(employee_data, "r") as f:
        employee = json.load(f)

# Base class for employees
class Employee:
    def __init__(self, username, full_name, email, department, cv_filename):
        self.username = username
        self.full_name = full_name
        self.email = email
        self.department = department
        self.cv_filename = cv_filename

#-------------------------------------------------Manager Class-----------------------------------------------------

class Manager(Employee):
    def __init__(self, username, full_name, email, department, cv_filename):
        super().__init__(username, full_name, email, department, cv_filename)

    def manager_menu(self, projects, leave_requests):
        while True:
            print("\n=== Manager Menu ===")
            print("1. Show Ongoing Projects")
            print("2. Add a Project")
            print("3. Employee List")
            print("4. Show Leave Requests")
            print("5. Show Company Details")
            print("6. Sign Out")

            choice = input("Enter your choice: ").strip()
            if choice == "1":
                self.show_ongoing_projects(projects)
            elif choice == "2":
                self.add_project(projects)
            elif choice == "3":
                self.show_employee_list(employee)  # Pass 'employee' to the method
            elif choice == "4":
                self.show_leave_requests(leave_requests)
            elif choice == "5":
                self.show_company_details()
            elif choice == "6":
                print(f"👋 Goodbye, {self.full_name}!")
                break
            else:
                print("⚠️ Invalid choice. Please try again.")

    def show_ongoing_projects(self, projects):
        print("\n=== Ongoing Projects ===")
        for project, employees in projects.items():
            print(f"- Project: {project}")
            print(f"  Assigned Employees: {', '.join(employees) if employees else 'None'}")
        print("---------------------------------")

    def add_project(self, projects):
        print("\n=== Add a New Project ===")
        project_name = input("Enter the project name: ").strip()
        if project_name in projects:
            print(f"⚠️ Project '{project_name}' already exists.")
        else:
            projects[project_name] = []
            print(f"✅ Project '{project_name}' added successfully.")
        print("---------------------------------")

    def show_employee_list(self, employees):
        print("\n=== Employee List ===")
        if not employees:
            print("⚠️ No employees found.")
        else:
            for i, (username, details) in enumerate(employees.items(), 1):
                print(f"{i}. Full Name: {details['full_name']}")
                print(f"   Email: {details['email']}")
                print(f"   Department: {details['department']}")
                assigned_projects = [project for project, emps in projects.items() if username in emps]
                print(f"   Working on: {', '.join(assigned_projects) if assigned_projects else 'None'}")
                print(f"   CV File: {details['cv_filename']}")
                print("---------")
        print("---------------------------------")

    def show_leave_requests(self, leave_requests):
        print("\n=== Leave Requests ===")
        if not leave_requests:
            print("⚠️ No leave requests found.")
        else:
            for i, request in enumerate(leave_requests, 1):
                print(f"{i}. Employee: {request['employee_name']}")
                print(f"   Reason: {request['reason']}")
                print("---------")

            while True:
                print("\nOptions:")
                print("1. Accept a leave request")
                print("2. Reject a leave request")
                print("3. Exit")
                choice = input("Enter your choice: ").strip()

                if choice == "1":
                    try:
                        index = int(input("Enter the leave request number to accept: ").strip())
                        if 1 <= index <= len(leave_requests):
                            accepted_request = leave_requests.pop(index - 1)
                            print(f"✅ Leave request from {accepted_request['employee_name']} has been accepted.")
                        else:
                            print("⚠️ Invalid request number.")
                    except ValueError:
                        print("⚠️ Please enter a valid number.")

                elif choice == "2":
                    try:
                        index = int(input("Enter the leave request number to reject: ").strip())
                        if 1 <= index <= len(leave_requests):
                            rejected_request = leave_requests.pop(index - 1)
                            print(f"🚫 Leave request from {rejected_request['employee_name']} has been rejected.")
                        else:
                            print("⚠️ Invalid request number.")
                    except ValueError:
                        print("⚠️ Please enter a valid number.")

                elif choice == "3":
                    print("Exiting leave request management.")
                    break

                else:
                    print("⚠️ Invalid choice. Please try again.")

        print("---------------------------------")

    def show_company_details(self):
        print("\n=== Company Details ===")
        company_name = "TechCorp"
        description = "A leading company in tech innovation and employee satisfaction."
        departments = np.array(["Manager", "Developer", "Designer", "Finance"])

        print(f"Company Name: {company_name}")
        print(f"Description: {description}")

        print(f"Departments: {', '.join(departments)}") 

        print(f"Number of Departments: {len(departments)}") 

        print(f"Company Name (Uppercase): {company_name.upper()}")

        company_location = ("Dhaka", "Bangladesh")
        print(f"Company Location: {', '.join(company_location)}")

        company_values = ["Innovation", "Integrity", "Customer Focus"]
        print(f"Company Values: {', '.join(company_values)}")

        print("---------------------------------") 

#-------------------------------------------------Developer Class-----------------------------------------------------

class Developer(Employee):
    def __init__(self, username, full_name, email, department, cv_filename):
        super().__init__(username, full_name, email, department, cv_filename)

    def developer_designer_menu(self, projects, leave_requests):
        while True:
            print(f"\n=== {self.department} Menu ===")
            print("1. Work on a Project")
            print("2. Request Leave")
            print("3. Sign Out")

            choice = input("Enter your choice: ").strip()
            if choice == "1":
                self.work_on_project(self.username, projects)
            elif choice == "2":
                self.request_leave(self.username, leave_requests)
            elif choice == "3":
                print(f"👋 Goodbye, {self.full_name}!")
                break
            else:
                print("⚠️ Invalid choice. Please try again.")

    def work_on_project(self, username, projects):
        print("\n=== Work on a Project ===")
        print("Available Projects:")
        for project in projects.keys():
            print(f"- {project}")

        project_name = input("Enter the project to work on: ").strip()
        if project_name in projects:
            if username in projects[project_name]:
                print(f"⚠️ You are already assigned to {project_name}.")
            else:
                projects[project_name].append(username)
                print(f"✅ Successfully assigned to {project_name}.")
        else:
            print("⚠️ Project not found.")

    def request_leave(self, username, leave_requests):
        print("\n=== Request Leave ===")
        reason = input("Enter the reason for leave: ").strip()
        leave_requests.append({
            "employee_name": employee[username]["full_name"],
            "reason": reason
        })
        print("✅ Leave request submitted successfully.")

    
#-------------------------------------------------Designer Class-----------------------------------------------------

class Designer(Employee):
    def __init__(self, username, full_name, email, department, cv_filename):
        super().__init__(username, full_name, email, department, cv_filename)

    def developer_designer_menu(self, projects, leave_requests):
        while True:
            print(f"\n=== {self.department} Menu ===")
            print("1. Work on a Project")
            print("2. Request Leave")
            print("3. Sign Out")

            choice = input("Enter your choice: ").strip()
            if choice == "1":
                self.work_on_project(self.username, projects)
            elif choice == "2":
                self.request_leave(self.username, leave_requests)
            elif choice == "3":
                print(f"👋 Goodbye, {self.full_name}!")
                break
            else:
                print("⚠️ Invalid choice. Please try again.")

    def work_on_project(self, username, projects):
        print("\n=== Work on a Project ===")
        print("Available Projects:")
        for project in projects.keys():
            print(f"- {project}")

        project_name = input("Enter the project to work on: ").strip()
        if project_name in projects:
            if username in projects[project_name]:
                print(f"⚠️ You are already assigned to {project_name}.")
            else:
                projects[project_name].append(username)
                print(f"✅ Successfully assigned to {project_name}.")
        else:
            print("⚠️ Project not found.")

    def request_leave(self, username, leave_requests):
        print("\n=== Request Leave ===")
        reason = input("Enter the reason for leave: ").strip()
        leave_requests.append({
            "employee_name": employee[username]["full_name"],
            "reason": reason
        })
        print("✅ Leave request submitted successfully.")



#-------------------------------------------------Finance Class-----------------------------------------------------
class Finance(Employee):
    def __init__(self, username, full_name, email, department, cv_filename):
        super().__init__(username, full_name, email, department, cv_filename)

    def finance_menu(self, salary_payments, department_salaries):
        while True:
            print("\n=== Finance Menu ===")
            print("1. Pay Salary")
            print("2. View Salary Payment Details")
            print("3. Sign Out")

            choice = input("Enter your choice: ").strip()
            if choice == "1":
                self.pay_salary(salary_payments, department_salaries)
            elif choice == "2":
                self.view_salary_details(salary_payments)
            elif choice == "3":
                print(f"👋 Goodbye, {self.full_name}!")
                break
            else:
                print("⚠️ Invalid choice. Please try again.")

    def pay_salary(self, salary_payments, department_salaries):
        print("\n=== Pay Salary ===")
        username = input("Enter the username of the employee to pay: ").strip()
        if username not in employee:
            print("⚠️ Employee not found. Please check the username.")
            return

        emp = employee[username]
        department = emp["department"]
        salary = department_salaries[department]

        salary_payments.append({
            "employee_name": emp["full_name"],
            "username": username,
            "email": emp["email"],
            "amount": salary,
            "department": department
        })
        print(f"💵 Salary of ${salary} has been paid to {emp['full_name']} ({department}).")

    def view_salary_details(self, salary_payments):
        print("\n=== Salary Payment Details ===")
        if not salary_payments:
            print("⚠️ No salary payments recorded.")
        else:
            for i, payment in enumerate(salary_payments, 1):
                print(f"{i}. {payment['employee_name']} ({payment['username']}) - ${payment['amount']}")
                print(f"   Email: {payment['email']}, Department: {payment['department']}")
                print("----------")

# -------------------------------------------------- Sign-Up --------------------------------------------------
def sign_up():
    print("\n=== Sign-Up Portal ===")
    while True:
        full_name = input("Enter your full name: ").strip()
        username = input("Choose a username: ").strip()
        if username in employee:
            print("⚠️ Username already exists. Please try a different one.")
            continue

        password = input("Create a password: ").strip()
        email = input("Enter your email address: ").strip()
        if "@" not in email:
            print("⚠️ Invalid email address. Please enter a valid email.")
            continue

        department = input(f"Select your department {departments}: ").strip()
        if department not in departments:
            print("⚠️ Invalid department. Please choose from the given options.")
            continue

        cv_filename = input("Upload your CV (enter file name): ").strip()
        if not os.path.exists(cv_filename):
            print("⚠️ CV file not found. Please upload a valid file.")
            continue

        # Save new user to the dictionary
        employee[username] = {
            "full_name": full_name,
            "password": password,
            "email": email,
            "department": department,
            "cv_filename": cv_filename
        }

        # Save data to file
        with open(employee_data, "w") as f:
            json.dump(employee, f)

        print(f"✅ Sign-Up successful! Welcome, {full_name}.")
        return username

# -------------------------------------------------- Log-In --------------------------------------------------
def log_in():
    print("\n=== Log-In Portal ===")
    while True:
        username = input("Enter your username: ").strip()
        password = input("Enter your password: ").strip()

        if username in employee and employee[username]["password"] == password:
            print(f"✅ Welcome back, {employee[username]['full_name']}!")
            return username
        else:
            print("⚠️ Invalid username or password. Please try again.")

#------------------------------------------------------Main Menu------------------------------------------
def main_menu():
    while True:
        print("\n=== TechCorp Pro ===")
        print("1. Log In")
        print("2. Sign Up")
        print("3. Exit")

        choice = input("Enter your choice: ").strip()

        if choice == "1":
            user = log_in()
            if user:
                dep = employee[user]["department"]
                if dep == "Manager":
                    employee_obj = Manager(user, employee[user]["full_name"], employee[user]["email"], dep, employee[user]["cv_filename"])
                    employee_obj.manager_menu(projects, leave_requests) 
                elif dep == "Finance":
                    employee_obj = Finance(user, employee[user]["full_name"], employee[user]["email"], dep, employee[user]["cv_filename"])
                    employee_obj.finance_menu(salary_payments, department_salaries) 
                elif dep in {"Developer", "Designer"}:
                    if dep == "Developer":
                        employee_obj = Developer(user, employee[user]["full_name"], employee[user]["email"], dep, employee[user]["cv_filename"])
                    else: 
                        employee_obj = Designer(user, employee[user]["full_name"], employee[user]["email"], dep, employee[user]["cv_filename"])
                    employee_obj.developer_designer_menu(projects, leave_requests) 
                else:
                    print("⚠️ Role-specific functionalities for your department are not yet implemented.")

        elif choice == "2":
            user = sign_up()
            if user:
                dep = employee[user]["department"]
                if dep == "Manager":
                    employee_obj = Manager(user, employee[user]["full_name"], employee[user]["email"], dep, employee[user]["cv_filename"])
                    employee_obj.manager_menu(projects, leave_requests) 
                elif dep == "Finance":
                    employee_obj = Finance(user, employee[user]["full_name"], employee[user]["email"], dep, employee[user]["cv_filename"])
                    employee_obj.finance_menu(salary_payments, department_salaries) 
                elif dep in {"Developer", "Designer"}:
                    if dep == "Developer":
                        employee_obj = Developer(user, employee[user]["full_name"], employee[user]["email"], dep, employee[user]["cv_filename"])
                    else: 
                        employee_obj = Designer(user, employee[user]["full_name"], employee[user]["email"], dep, employee[user]["cv_filename"])
                    employee_obj.developer_designer_menu(projects, leave_requests) 
                else:
                    print("⚠️ Role-specific functionalities for your department are not yet implemented.")

        elif choice == "3":
            print("👋 Goodbye! Have a great day!")
            break

        else:
            print("⚠️ Invalid choice. Please try again.")



if __name__ == "__main__":
    main_menu()