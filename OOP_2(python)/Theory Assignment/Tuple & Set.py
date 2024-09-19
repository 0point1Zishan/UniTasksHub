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