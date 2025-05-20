#Qs. 1:
# def sum(n):
#     if(n == 0):
#         return 0
#     return n + sum(n-1)

# print(sum(5))


#Qs 2:
def show(lst, idx):
    if(idx == len(lst)):
        return
    print(lst[idx], end=" ")
    show(lst, idx+1)

lst = ["Hello", "world", 'a', 'b', 'c', 1, 2, 5, 0.8, False, True, 100]
show(lst, 0)