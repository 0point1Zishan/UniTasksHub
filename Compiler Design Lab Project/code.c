#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX 500
#define MAXSTACK 200
#define MAXSYM 50
#define MAX_TOKENS 1000

char token_stream[5000];
int stream_idx = 0;
int line_number = 1;
int has_error = 0;

void addToStream(char *symbol) {
    strcat(token_stream, symbol);
    strcat(token_stream, " ");
    stream_idx = strlen(token_stream);
}

#define NUM_STATES 66
#define NUM_INPUTS 27

// DFA States
enum {
    S0,
    S1, S2, S3,                                 // int
    S4, S5, S6,                                 // dec  
    S7, S8, S9, S10, S11, S12,                  // return
    S13, S14, S15, S16, S17, S18,               // printf
    S19, S20, S21, S22, S23,                    // while
    S24, S25, S26, S27, S28,                    // break
    S29, S30, S31, S32,                         // main
    S33, S34, S35, S36,                         // variable
    S37, S38, S39, S40, S41, S42, S43, S44, S45,// loop label
    S46, S47, S48,                              // function
    S49, S50,                                   // number
    S51, S52, S53,                              // comment
    S54, S55,                                   // end stmt
    S56, S57, S58, S59, S60, S61, S62, S63, S64,// identifier
    DEAD
};

// Input mapping: char -> index
int get_input(char c) {
    if (c == '_') return 0;
    if (c == 'a') return 1;
    if (c == 'b') return 2;
    if (c == 'c') return 3;
    if (c == 'd') return 4;
    if (c == 'e') return 5;
    if (c == 'f') return 6;
    if (c == 'h') return 7;
    if (c == 'i') return 8;
    if (c == 'k') return 9;
    if (c == 'l') return 10;
    if (c == 'm') return 11;
    if (c == 'n') return 12;
    if (c == 'o') return 13;
    if (c == 'p') return 14;
    if (c == 'r') return 15;
    if (c == 't') return 16;
    if (c == 'u') return 17;
    if (c == 'w') return 18;
    if (c == 'F') return 19;
    if (isalpha(c)) return 20;
    if (isdigit(c)) return 21;
    if (c == ':') return 22;
    if (c == '.') return 23;
    if (c == '/') return 24;
    if (c == ' ' || c == '\t') return 25;
    return 26;
}

// Accepting states with token labels
const char* accepting_tokens[NUM_STATES] = {
    NULL, NULL, NULL, "KEYWORD_INT",
    NULL, NULL, "KEYWORD_DEC",
    NULL, NULL, NULL, NULL, NULL, "KEYWORD_RETURN",
    NULL, NULL, NULL, NULL, NULL, "KEYWORD_PRINTF",
    NULL, NULL, NULL, NULL, "KEYWORD_WHILE",
    NULL, NULL, NULL, NULL, "KEYWORD_BREAK",
    NULL, NULL, NULL, "KEYWORD_MAIN",
    NULL, NULL, NULL, "VARIABLE",
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "LOOP_LABEL",
    NULL, NULL, "FUNCTION",
    NULL, "NUMBER",
    NULL, NULL, "COMMENT",
    NULL, "END_STMT",
    "IDENTIFIER", "IDENTIFIER", "IDENTIFIER", "IDENTIFIER", "IDENTIFIER",
    "IDENTIFIER", "IDENTIFIER", "IDENTIFIER", "IDENTIFIER",
    NULL
};

// Token stream symbols
const char* stream_symbols[NUM_STATES] = {
    NULL, NULL, NULL, "t",
    NULL, NULL, "t",
    NULL, NULL, NULL, NULL, NULL, "r",
    NULL, NULL, NULL, NULL, NULL, "p",
    NULL, NULL, NULL, NULL, "w",
    NULL, NULL, NULL, NULL, "b",
    NULL, NULL, NULL, "m",
    NULL, NULL, NULL, "v",
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "l",
    NULL, NULL, "f",
    NULL, "n",
    NULL, NULL, NULL,
    NULL, "s",
    "i", "i", "i", "i", "i", "i", "i", "i", "i",
    NULL
};

int next_state[NUM_STATES][NUM_INPUTS] = {
/* a    b    c    d    e    f    h    i    k    l    m    n    o    p    r    t    u    w    F   alp  dig   :    .    /   sp  oth */
{ S33, S46, S24, S46, S4,  S46, S46, S46, S1,  S46, S37, S29, S46, S46, S13, S7,  S46, S46, S19, S46, S46, S49, DEAD,S54, S51, DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S2,  S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S3,  S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S5,  S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S6,  S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S8,  S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S9,  S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S10, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S11, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S12, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S14, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S15, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S16, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S17, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S18, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S20, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S21, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S22, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S23, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S25, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S26, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S27, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S28, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S30, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S31, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S32, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ DEAD,S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ DEAD,S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S34, S35, DEAD,DEAD,DEAD,DEAD,DEAD },
{ DEAD,S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, S36, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S38, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S39, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S40, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S41, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ DEAD,S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ DEAD,S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S42, S43, DEAD,DEAD,DEAD,DEAD,DEAD },
{ DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,S44, DEAD,DEAD,DEAD,DEAD,DEAD },
{ DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,S45, DEAD,DEAD,DEAD,DEAD },
{ DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S47, S46, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S48, S46, S46, S46, S46, S46, S46, S47, S46, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S46, S47, S46, DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,S50, DEAD,DEAD,DEAD,DEAD,DEAD },
{ DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,S50, DEAD,DEAD,DEAD,DEAD,DEAD },
{ DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,S52, DEAD,DEAD },
{ DEAD,S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, DEAD,DEAD,DEAD,DEAD,S53, DEAD },
{ DEAD,S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, S53, DEAD,DEAD,DEAD,DEAD,S53, DEAD },
{ DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,S55, DEAD,DEAD,DEAD },
{ DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, S57, DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, S57, DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, S57, DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, S57, DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, S57, DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, S57, DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, S57, DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, S57, DEAD,DEAD,DEAD,DEAD,DEAD },
{ S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S56, S47, S56, S57, DEAD,DEAD,DEAD,DEAD,DEAD },
{ DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD,DEAD }
};


const char* run_dfa(char *str, const char** stream_sym) {
    int state = S0;
    
    for (int i = 0; str[i] != '\0'; i++) {
        int input = get_input(str[i]);
        
        if (input == 26) {
            *stream_sym = "i";
            return "INVALID_TOKEN";
        }
        
        state = next_state[state][input];
        
        if (state == DEAD) {
            *stream_sym = "i";
            return "INVALID_IDENTIFIER";
        }
    }
    
    if (accepting_tokens[state] != NULL) {
        *stream_sym = stream_symbols[state];
        return accepting_tokens[state];
    } else {
        *stream_sym = "i";
        return "INVALID_IDENTIFIER";
    }
}

// Header DFA
const char* run_header_dfa(char *str) {
    const char *expected = "#include <stdio.h>";
    
    int state = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == expected[i]) {
            state++;
        } else {
            return NULL;
        }
    }
    
    if (strcmp(str, expected) == 0) {
        return "HEADER";
    }
    return NULL;
}

// Comment DFA
const char* run_comment_dfa(char *str) {
    int state = 0;
    
    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        
        if (state == 0 && c == '/') state = 1;
        else if (state == 1 && c == '/') state = 2;
        else if (state == 2 && (isalpha(c) || c == ' ' || c == '\t')) state = 2;
        else state = -1;
        
        if (state == -1) {
            return NULL;
        }
    }
    
    if (state == 2) {
        return "COMMENT";
    }
    return NULL;
}

// Number DFA
const char* run_number_dfa(char *str) {
    int state = 0;
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (isdigit(str[i])) {
            state = 1;
        } else {
            return NULL;
        }
    }
    
    if (state == 1) {
        return "NUMBER";
    }
    return NULL;
}

// End Statement
const char* run_endstmt_dfa(char *str) {
    int state = 0;
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.' && state < 2) {
            state++;
        } else {
            return NULL;
        }
    }
    
    if (state == 2) {
        return "END_STMT";
    }
    return NULL;
}

// ==========================================
//! Lexical Analyzer
// ==========================================
void lexicalAnalyzer(FILE *fp) {
    char c;
    char token[MAX];
    int k = 0;
    
    printf("Lexical Analysis Output:\n\n");

    while ((c = fgetc(fp)) != EOF) {
        
        if (c == '\n') { line_number++; continue; }
        
        if (c == '#') {
            k = 0;
            token[k++] = c;
            while ((c = fgetc(fp)) != '\n' && c != EOF && k < MAX-1) token[k++] = c;
            token[k] = '\0';
            
            if (run_header_dfa(token) != NULL) {
                printf("Token: %-20s \tType: HEADER\n", token);
                addToStream("h");
            } else {
                printf("Token: %-20s \tType: INVALID_HEADER\n", token);
                has_error = 1;
            }
            line_number++;
            continue;
        }

        if (isspace(c)) continue;

        if (c == '/') {
            k = 0;
            token[k++] = c;
            c = fgetc(fp);
            
            if (c == '/') {
                // single line comment
                token[k++] = c;
                while ((c = fgetc(fp)) != '\n' && c != EOF && k < MAX-1) token[k++] = c;
                token[k] = '\0';
                
                
                if (c == '\n') line_number++;
            } else {
                token[k] = '\0';
                printf("Token: %-20s \tType: OPERATOR\n", token);
                addToStream("/");
                if (c != EOF) ungetc(c, fp);
            }
            continue;
        }

        if (c == '"') {
            k = 0;
            token[k++] = '"';
            while ((c = fgetc(fp)) != EOF && k < MAX - 1) {
                if (c == '"') { token[k++] = '"'; break; }
                else if (c == '\\') {
                    token[k++] = c;
                    c = fgetc(fp);
                    if (c != EOF && k < MAX - 1) token[k++] = c;
                } else token[k++] = c;
            }
            token[k] = '\0';
            printf("Token: %-20s \tType: STRING_LITERAL\n", token);
            addToStream("q");
            continue;
        }

        if (isalpha(c) || c == '_') {
            k = 0;
            token[k++] = c;
            
            while ((c = fgetc(fp)) != EOF && k < MAX - 1) {
                if (isalnum(c) || c == '_') token[k++] = c;
                else if (c == ':') { token[k++] = c; break; }
                else { ungetc(c, fp); break; }
            }
            token[k] = '\0';

            const char* stream_sym;
            const char* token_type = run_dfa(token, &stream_sym);
            
            printf("Token: %-20s \tType: %s\n", token, token_type);
            if (stream_sym != NULL) addToStream((char*)stream_sym);
            if (strstr(token_type, "INVALID") != NULL) has_error = 1;
            continue;
        }

        if (isdigit(c)) {
            k = 0;
            token[k++] = c;
            while ((c = fgetc(fp)) != EOF && isdigit(c) && k < MAX - 1) token[k++] = c;
            token[k] = '\0';
            if (c != EOF) ungetc(c, fp);
            
            if (run_number_dfa(token) != NULL) {
                printf("Token: %-20s \tType: NUMBER\n", token);
                addToStream("n");
            } else {
                printf("Token: %-20s \tType: INVALID_NUMBER\n", token);
                has_error = 1;
            }
            continue;
        }

        if (c == '.') {
            char nextC = fgetc(fp);
            if (nextC == '.') {
                if (run_endstmt_dfa("..") != NULL) {
                    printf("Token: %-20s \tType: END_STMT\n", "..");
                    addToStream("s");
                }
            } else {
                if (nextC != EOF) ungetc(nextC, fp);
                token[0] = c; token[1] = '\0';
                printf("Token: %-20s \tType: SPECIAL_SYMBOL\n", token);
                addToStream(".");
            }
            continue;
        }

        token[0] = c; token[1] = '\0';
        printf("Token: %-20s \tType: SPECIAL_SYMBOL\n", token);
        
        if (c == '{') addToStream("{");
        else if (c == '}') addToStream("}");
        else if (c == '(') addToStream("(");
        else if (c == ')') addToStream(")");
        else if (c == '=') addToStream("=");
        else if (c == '+') addToStream("+");
        else if (c == '-') addToStream("-");
        else if (c == '*') addToStream("*");
        else if (c == '<') addToStream("<");
        else if (c == '>') addToStream(">");
        else if (c == ',') addToStream(",");
        else if (c == ';') addToStream(";");
    }
}

// ==========================================
//! PARSER
// ==========================================

char *NT[] = {"S", "A", "D", "B", "T", "E", "X"};
#define NNT 7

char *TERMINALS[] = {"h", "t", "f", "m", "v", "n", "p", "l", "w", "s", "r", "b", "(", ")", "{", "}", "=", "<", "+", "-", "*", ",", "q", "i", "$"};
#define NTER 25

char *RHS[] = {
    "h A", "t D", "", "f ( t v ) { B r E s } A", "m ( ) { B r E s }",
    "T B", "", "t v = E s", "p ( q , v ) s", "l w ( t v < n s ) { B b s }",
    "n", "v X", "f ( v )", "+ E", "- E", "* E", ""
};

int TABLE[NNT][NTER] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3},
    {0, 0, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 6, 0, 0, 0, 0, 6, 6, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 8, 0, 0, 0, 0, 9,10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0,13, 0,12,11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0,17, 0, 0, 0,17, 0,17, 0, 0,14,15,16, 0, 0, 0, 0}
};

char parser_stack[MAXSTACK][MAXSYM];
int parser_top = -1;

void parser_push(char *s) { strcpy(parser_stack[++parser_top], s); }
char* parser_pop() { return (parser_top >= 0) ? parser_stack[parser_top--] : NULL; }

void print_stack() {
    printf("[");
    for (int i = parser_top; i >= 0; i--) {
        printf("%s", parser_stack[i]);
        if (i > 0) printf(", ");
    }
    printf("]");
}

int find_nt(char *x) { for (int i = 0; i < NNT; i++) if (strcmp(NT[i], x) == 0) return i; return -1; }
int find_t(char *x) { for (int i = 0; i < NTER; i++) if (strcmp(TERMINALS[i], x) == 0) return i; return -1; }

int tokenize_parser(char *line, char tokens[][MAXSYM]) {
    int n = 0;
    char temp[5000];
    strcpy(temp, line);
    char *p = strtok(temp, " \t\n");
    while (p) { strcpy(tokens[n++], p); p = strtok(NULL, " \t\n"); }
    if (n == 0 || strcmp(tokens[n-1], "$") != 0) strcpy(tokens[n++], "$");
    return n;
}

void runParser(char *input_string) {
    char input[MAX_TOKENS][MAXSYM];
    int n = tokenize_parser(input_string, input);

    printf("\nTokens detected:\n");
    for (int i = 0; i < n; i++) printf("%s ", input[i]);
    printf("\n\n");

    parser_top = -1;
    parser_push("$");
    parser_push(NT[0]);
    int ip = 0;

    printf("%-25s %-10s %-10s %-25s\n", "Stack", "Lookahead", "Top", "Production Applied");
    printf("-------------------------------------------------------------------\n");

    while (parser_top >= 0) {
        char X[MAXSYM];
        strcpy(X, parser_stack[parser_top]);
        parser_pop();
        char *a = input[ip];

        printf("%-25s %-10s %-10s ", "", a, X);

        if (strcmp(X, "$") == 0) {
            if (strcmp(a, "$") == 0) {
                printf("%-25s\n", "match");
                print_stack();
                printf("\n\nACCEPTED\n");
                return;
            }
            printf("REJECTED\n");
            return;
        }

        int tindex = find_t(X);
        if (tindex != -1) {
            if (strcmp(X, a) == 0) {
                printf("%-25s\n", "match");
                ip++;
            } else {
                printf("REJECTED\n");
                return;
            }
            print_stack();
            printf("\n");
            continue;
        }

        int ntindex = find_nt(X);
        int aindex = find_t(a);

        if (ntindex == -1 || aindex == -1) {
            printf("REJECTED\n");
            return;
        }

        int prod = TABLE[ntindex][aindex];
        if (prod == 0) {
            printf("REJECTED\nNo rule for (%s,%s)\n", X, a);
            return;
        }

        if (strlen(RHS[prod-1]) == 0)
            printf("%-25s\n", "epsilon");
        else
            printf("%-25s\n", RHS[prod-1]);

        if (strlen(RHS[prod-1]) > 0) {
            char temp[500];
            strcpy(temp, RHS[prod-1]);
            char *p = strtok(temp, " ");
            char symbols[50][MAXSYM];
            int k = 0;
            while (p) { strcpy(symbols[k++], p); p = strtok(NULL, " "); }
            for (int i = k-1; i >= 0; i--) parser_push(symbols[i]);
        }
        print_stack();
        printf("\n");
    }

    if (strcmp(input[ip], "$") == 0)
        printf("\nACCEPTED\n");
    else
        printf("\nREJECTED: input not fully consumed\n");
}

// ==========================================
//! Main Function
// ==========================================

int main() {
    char filename[260];

    while (1) {
        printf("\nEnter filename (or 'exit'): ");
        
        if (scanf("%259s", filename) != 1) return 1;
        if (strcmp(filename, "exit") == 0) { printf("Goodbye!\n"); break; }

        FILE *fp = fopen(filename, "r");
        if (!fp) { printf("Cannot open file '%s'\n", filename); continue; }

        token_stream[0] = '\0';
        stream_idx = 0;
        line_number = 1;
        has_error = 0;

        lexicalAnalyzer(fp);
        fclose(fp);

        printf("\nToken Stream: %s\n", token_stream);

        if (has_error) printf("\n[WARNING] Lexical errors detected!\n");

        runParser(token_stream);
    }

    return 0;
}