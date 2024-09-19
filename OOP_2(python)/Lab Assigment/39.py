import random
import string

length = 8
random_str = ''.join(random.choices(string.ascii_letters + string.digits, k=length))
print(f"Random string: {random_str}")
