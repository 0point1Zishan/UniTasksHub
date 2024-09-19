import string

my_str = "Hello!!!, welcome??? to -- Python."
no_punct = "".join([char for char in my_str if char not in string.punctuation])
print(no_punct)
