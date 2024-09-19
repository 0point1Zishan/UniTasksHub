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