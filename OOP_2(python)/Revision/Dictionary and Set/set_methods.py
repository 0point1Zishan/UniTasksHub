st = {4, 3, "world", 1,2, "Hello", 3, 4,5}
st.add(10)
print(st)
st.add((1.5, 2.5, 3.2, 4.8))
print(st)

#st.add([1, 2, 3, 4, 5])   # unable type hash

st.remove("world")
print(st)

       
print(st.pop())  # remove a random value and return 

print(len(st))
st.clear()
print(len(st))

st1 = {1, 2, 4, 5, 7, 10, 12}
st2 = {4, 8, 9}
print(st1.union(st2))       #return combine both set
print(st1.intersection(st2))   # return common from both