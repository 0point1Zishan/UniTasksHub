def manage_string():
    sentence = "Learning Python is fun and rewarding."
    
    substring = sentence[-26:-11]  # "Python is fun"
    print(f"Extracted substring: {substring}")
    
    modified_sentence = sentence.replace("rewarding", "exciting")
    print(f"Modified sentence: {modified_sentence}")
    
    insertion = " Keep practicing!"
    position = modified_sentence.find("exciting") + len("exciting")
    final_sentence = modified_sentence[:position] + insertion + modified_sentence[position:]
    print(f"Final sentence with insertion: {final_sentence}")
    
    capitalized_sentence = final_sentence.title()
    print(f"Capitalized sentence: {capitalized_sentence}")
    
    return capitalized_sentence

manage_string()
