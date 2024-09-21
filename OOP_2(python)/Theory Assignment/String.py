def manage_string():
    sentence = "Learning Python is fun and rewarding."
    
    substring = sentence[-28:-15]
    print(f"Extracted substring: {substring}")
    
    sentence = sentence.replace("rewarding", "exciting")
    print(f"Modified sentence: {sentence}")
    
    exciting_index = sentence.find("exciting")
    
    if exciting_index != -1:
        insert_sen = " Keep practicing!"
        exciting_end = exciting_index + len("exciting")
        sentence = sentence[:exciting_end] + insert_sen + sentence[exciting_end:]
    
    print(f"Extended sentence: {sentence}")
    
    sentence.title()
    print(f"Capitalized sentence: {sentence}")
    
    return sentence

manage_string()
