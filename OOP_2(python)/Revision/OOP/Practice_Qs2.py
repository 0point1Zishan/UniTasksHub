class Account:
    def __init__(self, balance, account_no):
        self.balance = balance
        self.account_no = account_no
    
    def debit(self, amount):
        self.balance -= amount

    def credit(self, amount):
        self.balance += amount

    def  printBalance(self):
        print(f"Current balance {self.balance}.")


acc1 = Account(100000, 1234)
acc1.debit(3000)
acc1.credit(10000)
acc1.printBalance()