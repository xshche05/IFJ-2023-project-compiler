#include "lexer.h"
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>


char control_char = ' ';
file_t *global_file = NULL;


token_t *token_init(token_type_t type, token_subtype_t subtype, string_t *lexeme) {
    token_t *token = malloc(sizeof(token_t));
    if (token == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        return NULL;
    }
    token->type = type;
    token->subtype = subtype;
    token->lexeme = string_copy(lexeme);
    return token;
}

void token_dtor(token_t *token) {
    if (token == NULL) {
        return;
    }
    string_dtor(token->lexeme);
    free(token);
    token = NULL;
}

token_array_t *token_array_init() {
    token_array_t *token_array = malloc(sizeof(token_array_t));
    if (token_array == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        return NULL;
    }
    token_array->array = malloc(sizeof(token_t *) * 16);
    if (token_array->array == NULL) {
        free(token_array);
        fprintf(stderr, "Error: malloc failed.\n");
        return NULL;
    }
    token_array->allocated = 16;
    token_array->length = 0;
    return token_array;
}

void token_array_dtor(token_array_t *token_array) {
    if (token_array == NULL) {
        return;
    }
    for (size_t i = 0; i < token_array->length; i++) {
        token_dtor(token_array->array[i]);
    }
    free(token_array->array);
    free(token_array);
    token_array = NULL;
}

int token_array_add(token_array_t *token_array, token_t *token) {
    if (token_array == NULL || token == NULL) {
        return -1;
    }
    if (token_array->length >= token_array->allocated) {
        void *tmp = realloc(token_array->array, sizeof(token_t *) * token_array->allocated * 2);
        if (tmp == NULL) {
            return -1;
        }
        token_array->array = tmp;
        token_array->allocated *= 2;
    }
    token_array->array[token_array->length] = token;
    token_array->length++;
    return 0;
}

int add_token(token_array_t *token_array, token_type_t type, token_subtype_t subtype, string_t *lexeme) {
    token_t *token = token_init(type, subtype, lexeme);
    if (!isspace(control_char)) file_back_step(global_file);
    return token_array_add(token_array, token);
}

void print_token(token_t *token) {
    if (token == NULL) {
        return;
    }
    printf("Token = ");
    printf("type: ");
    switch (token->type) {
        case TOKEN_KEYWORD:
            printf("KEYWORD");
            break;
        case TOKEN_LITERAL:
            printf("LITERAL");
            break;
        case TOKEN_IDENTIFIER:
            printf("IDENTIFIER");
            break;
        case TOKEN_OPERATOR:
            printf("OPERATOR");
            break;
        case TOKEN_PUNCTUATOR:
            printf("TOKEN_PUNCTUATOR");
            break;
        default:
            printf("UNKNOWN");
    }
    if (token->lexeme != NULL) printf(", lexeme: \"%s\"\n", token->lexeme->str);
}

int is_keyword(string_t *lexeme) {
    if (strcmp(lexeme->str, "Double") == 0) return DOUBLE_TYPE;
    else if (strcmp(lexeme->str, "else") == 0) return ELSE;
    else if (strcmp(lexeme->str, "func") == 0) return FUNC;
    else if (strcmp(lexeme->str, "if") == 0) return IF;
    else if (strcmp(lexeme->str, "Int") == 0) return INT_TYPE;
    else if (strcmp(lexeme->str, "let") == 0) return LET;
    else if (strcmp(lexeme->str, "return") == 0) return RETURN;
    else if (strcmp(lexeme->str, "String") == 0) return STRING_TYPE;
    else if (strcmp(lexeme->str, "var") == 0) return VAR;
    else if (strcmp(lexeme->str, "while") == 0) return WHILE;
    else if (strcmp(lexeme->str, "Bool") == 0) return BOOL_TYPE;
    else if (strcmp(lexeme->str, "nil") == 0) return NIL_LITERAL;
    return -1;
}

token_array_t *source_code_to_tokens(file_t *file) {

    // Initial values DO NOT CHANGE
    global_file = file;
    fsm_state_t fsm_state = START_S;
    token_array_t *t_array = token_array_init();
    bool multiline = false;
    string_t *lexeme = string_init();
    token_subtype_t subtype;
    token_type_t type;
    char c, prev = ' ', count = 0;
    int keyword_code;

    // FSM loop
    while ((c = file_getc(file)) != EOF) {
        control_char = c;
        switch (fsm_state) {
            case START_S:
                switch (c) {
                    // White symbols
                    case 9 ... 13:
                    case 32:
                        break;
                        // String literals
                    case '"':
                        fsm_state = STR_START_S;
                        string_add_char(lexeme, c);
                        break;
                        // Number literals
                    case '0' ... '9':
                        fsm_state = INTEGER_S;
                        string_add_char(lexeme, c);
                        break;
                        // Operators
                    case '+':
                        fsm_state = PLUS_S;
                        string_add_char(lexeme, c);
                        break;
                    case '*':
                        fsm_state = MUL_S;
                        string_add_char(lexeme, c);
                        break;
                    case '!':
                        fsm_state = EXCL_MARK_S;
                        string_add_char(lexeme, c);
                        break;
                    case '?':
                        fsm_state = NILABLE_S;
                        string_add_char(lexeme, c);
                        break;
                    case '>':
                        fsm_state = GREATER_S;
                        string_add_char(lexeme, c);
                        break;
                    case '<':
                        fsm_state = LESS_S;
                        string_add_char(lexeme, c);
                        break;
                    case '-':
                        fsm_state = MINUS_S;
                        string_add_char(lexeme, c);
                        break;
                    case '=':
                        fsm_state = ASSIGN_S;
                        string_add_char(lexeme, c);
                        break;
                    case '{':
                        fsm_state = BRACE_L_S;
                        string_add_char(lexeme, c);
                        break;
                    case '}':
                        fsm_state = BRACE_R_S;
                        string_add_char(lexeme, c);
                        break;
                    case '(':
                        fsm_state = BRACKET_L_S;
                        string_add_char(lexeme, c);
                        break;
                    case ')':
                        fsm_state = BRACKET_R_S;
                        string_add_char(lexeme, c);
                        break;
                    case ':':
                        fsm_state = COLON_S;
                        string_add_char(lexeme, c);
                        break;
                    case ';':
                        fsm_state = SEMICOLON_S;
                        string_add_char(lexeme, c);
                        break;
                    case ',':
                        fsm_state = COMMA_S;
                        string_add_char(lexeme, c);
                        break;
                    case '/':
                        fsm_state = DIV_S;
                        string_add_char(lexeme, c);
                        break;
                    case '_':
                    case 'a' ... 'z':
                    case 'A' ... 'Z':
                        fsm_state = ID_S;
                        string_add_char(lexeme, c);
                        break;
                        // Other
                    default:
                        return NULL;
                }
                break;
            case PLUS_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = ADDITION;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case MUL_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = MULTIPLICATION;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case EXCL_MARK_S:
                if (c == '=') {
                    fsm_state = NOT_EQUAL_S;
                    string_add_char(lexeme, c);
                } else {
                    type = TOKEN_OPERATOR;
                    if (isspace(prev)) subtype.operator_type = LOGICAL_NOT;
                    else subtype.operator_type = UNWRAP_NILABLE;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    string_clear(lexeme);
                }
                break;
            case NOT_EQUAL_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = NOT_EQUAL_TO;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case NILABLE_S:
                if (c == '?') {
                    fsm_state = IS_NIL_S;
                    string_add_char(lexeme, c);
                } else {
                    type = TOKEN_OPERATOR;
                    subtype.operator_type = NILABLE;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    string_clear(lexeme);
                }
                break;
            case IS_NIL_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = IS_NIL;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case GREATER_S:
                if (c == '=') {
                    fsm_state = GREATER_EQUAL_S;
                    string_add_char(lexeme, c);
                } else {
                    type = TOKEN_OPERATOR;
                    subtype.operator_type = GREATER_THAN;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    string_clear(lexeme);
                }
                break;
            case GREATER_EQUAL_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = GREATER_THAN_OR_EQUAL_TO;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case LESS_S:
                if (c == '=') {
                    fsm_state = LESS_EQUAL_S;
                    string_add_char(lexeme, c);
                } else {
                    type = TOKEN_OPERATOR;
                    subtype.operator_type = LESS_THAN;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    string_clear(lexeme);
                }
                break;
            case LESS_EQUAL_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = LESS_THAN_OR_EQUAL_TO;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case MINUS_S:
                if (c == '>') {
                    fsm_state = ARROW_S;
                    string_add_char(lexeme, c);
                } else {
                    type = TOKEN_OPERATOR;
                    subtype.operator_type = SUBTRACTION;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    string_clear(lexeme);
                }
                break;
            case ARROW_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = ARROW;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case ASSIGN_S:
                if (c == '=') {
                    fsm_state = EQUAL_S;
                    string_add_char(lexeme, c);
                } else {
                    type = TOKEN_OPERATOR;
                    subtype.operator_type = ASSIGNMENT;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    string_clear(lexeme);
                }
                break;
            case EQUAL_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = EQUAL_TO;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case BRACE_L_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = LEFT_BRACE;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case BRACE_R_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = RIGHT_BRACE;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case BRACKET_L_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = LEFT_BRACKET;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case BRACKET_R_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = RIGHT_BRACKET;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case COLON_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = COLON;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case SEMICOLON_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = SEMICOLON;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case COMMA_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = COMMA;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case INTEGER_S:
                switch (c) {
                    case '0' ... '9':
                        string_add_char(lexeme, c);
                        break;
                    case 'e':
                    case 'E':
                        fsm_state = REAL_E_S;
                        string_add_char(lexeme, c);
                        break;
                    case '.':
                        fsm_state = REAL_S;
                        string_add_char(lexeme, c);
                        break;
                    default:
                        type = TOKEN_LITERAL;
                        subtype.literal_type = INTEGER_LITERAL;
                        add_token(t_array, type, subtype, lexeme);
                        fsm_state = START_S;
                        string_clear(lexeme);
                }
                break;
            case ID_S:
                switch (c) {
                    case '_':
                    case 'a' ... 'z':
                    case 'A' ... 'Z':
                    case '0' ... '9':
                        string_add_char(lexeme, c);
                        break;
                    default:
                        keyword_code = is_keyword(lexeme);
                        switch (keyword_code) {
                            case -1:
                                type = TOKEN_IDENTIFIER;
                                break;
                            case NIL_LITERAL:
                                type = TOKEN_LITERAL;
                                subtype.literal_type = NIL_LITERAL;
                                break;
                            default:
                                type = TOKEN_KEYWORD;
                                subtype.keyword_type = keyword_code;
                        }
                        add_token(t_array, type, subtype, lexeme);
                        fsm_state = START_S;
                        string_clear(lexeme);
                }
                break;
            case REAL_S:
                if ('0' <= c && c <= '9') {
                    fsm_state = REAL_NUM_S;
                    string_add_char(lexeme, c);
                } else {
                    return NULL;
                }
                break;
            case REAL_NUM_S:
                switch (c) {
                    case '0' ... '9':
                        string_add_char(lexeme, c);
                        break;
                    case 'e':
                    case 'E':
                        fsm_state = REAL_E_S;
                        string_add_char(lexeme, c);
                        break;
                    default:
                        type = TOKEN_LITERAL;
                        subtype.literal_type = REAL_LITERAL;
                        add_token(t_array, type, subtype, lexeme);
                        fsm_state = START_S;
                        string_clear(lexeme);
                }
                break;
            case REAL_E_S:
                switch (c) {
                    case '0' ... '9':
                        fsm_state = REAL_EXP_S;
                        string_add_char(lexeme, c);
                        break;
                    case '+':
                    case '-':
                        fsm_state = EXP_SIGN_S;
                        string_add_char(lexeme, c);
                        break;
                    default:
                        return NULL;
                }
                break;
            case EXP_SIGN_S:
                if ('0' <= c && c <= '9') {
                    fsm_state = REAL_EXP_S;
                    string_add_char(lexeme, c);
                } else {
                    return NULL;
                }
                break;
            case REAL_EXP_S:
                if ('0' <= c && c <= '9') {
                    string_add_char(lexeme, c);
                } else {
                    type = TOKEN_LITERAL;
                    subtype.literal_type = REAL_LITERAL;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    string_clear(lexeme);
                }
                break;
                // string literals
            case STR_START_S:
                switch (c) {
                    case '"':
                        fsm_state = STR_1_END_S;
                        string_add_char(lexeme, c);
                        break;
                    case '\\':
                        fsm_state = ESC_S;
                        string_add_char(lexeme, c);
                        break;
                    case 31 ... 33:
                    case 35 ... 91:
                    case 93 ... 126:
                        string_add_char(lexeme, c);
                        break;
                    default:
                        return NULL;
                }
                break;
            case STR_1_END_S:
                if (c == '"') {
                    fsm_state = STR_2_START_S;
                    string_add_char(lexeme, c);
                } else {
                    type = TOKEN_LITERAL;
                    subtype.literal_type = STRING_LITERAL;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    string_clear(lexeme);
                }
                break;
            case STR_2_START_S:
                if (c == '\n') {
                    fsm_state = STR_MULT_S;
                    multiline = true;
                    string_add_char(lexeme, c);
                } else {
                    return NULL;
                }
                break;
            case STR_MULT_S:
                switch (c) {
                    case '\n':
                        string_add_char(lexeme, c);
                        fsm_state = STR_LF_S;
                        break;
                    case '\\':
                        string_add_char(lexeme, c);
                        fsm_state = ESC_S;
                        break;
                    case 31 ... 91:
                    case 93 ... 126:
                        string_add_char(lexeme, c);
                        break;
                    default:
                        return NULL;
                }
                break;
            case STR_LF_S:
                switch (c) {
                    case '"':
                        string_add_char(lexeme, c);
                        fsm_state = STR_2_E_S;
                        break;
                    case 31 ... 33:
                    case 35 ... 126:
                        string_add_char(lexeme, c);
                        fsm_state = STR_MULT_S;
                        break;
                    default:
                        return NULL;
                }
                break;
            case STR_2_E_S:
                switch (c) {
                    case '"':
                        string_add_char(lexeme, c);
                        fsm_state = STR_2_EE_S;
                        break;
                    case 31 ... 33:
                    case 35 ... 126:
                        string_add_char(lexeme, c);
                        fsm_state = STR_MULT_S;
                        break;
                    default:
                        return NULL;
                }
                break;
            case STR_2_EE_S:
                switch (c) {
                    case '"':
                        string_add_char(lexeme, c);
                        fsm_state = STR_2_END_S;
                        break;
                    case 31 ... 33:
                    case 35 ... 126:
                        string_add_char(lexeme, c);
                        fsm_state = STR_MULT_S;
                        break;
                    default:
                        return NULL;
                }
                break;
            case STR_2_END_S:
                type = TOKEN_LITERAL;
                subtype.literal_type = STRING_LITERAL;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                string_clear(lexeme);
                break;
            case ESC_S:
                switch (c) {
                    case '"':
                    case 'n':
                    case 'r':
                    case 't':
                    case '\\':
                        string_add_char(lexeme, c);
                        fsm_state = multiline ? STR_MULT_S : STR_START_S;
                        break;
                    case 'u':
                        string_add_char(lexeme, c);
                        fsm_state = U_SEC_S;
                        break;
                    case '(':  // TODO interpolation
                    default:
                        return NULL;
                }
                break;
            case U_SEC_S:
                if (c == '{') {
                    string_add_char(lexeme, c);
                    fsm_state = U_SEC_START_S;
                } else {
                    return NULL;
                }
                break;
            case U_SEC_START_S:
                switch (c) {
                    case '0' ... '9':
                    case 'a' ... 'f':
                    case 'A' ... 'F':
                        string_add_char(lexeme, c);
                        fsm_state = U_SEC_NUM_S;
                        count = 1;
                        break;
                    default:
                        return NULL;
                }
                break;
            case U_SEC_NUM_S:
                if (count >= 8) return NULL;
                switch (c) {
                    case '0' ... '9':
                    case 'a' ... 'f':
                    case 'A' ... 'F':
                        string_add_char(lexeme, c);
                        fsm_state = U_SEC_NUM_S;
                        count++;
                        break;
                    case '}':
                        string_add_char(lexeme, c);
                        fsm_state = multiline ? STR_MULT_S : STR_START_S;
                        break;
                    default:
                        return NULL;
                }
                break;
                // comments
            case DIV_S:
                switch (c) {
                    case '/':
                        fsm_state = COM_SINGL_S;
                        break;
                    case '*':
                        fsm_state = COM_MULT_S;
                        break;
                    default:
                        type = TOKEN_OPERATOR;
                        subtype.operator_type = DIVISION;
                        add_token(t_array, type, subtype, lexeme);
                        fsm_state = START_S;
                        string_clear(lexeme);
                }
                break;
            case COM_SINGL_S:
                if (c == '\n') {
                    fsm_state = COM_END_S;
                }
                break;
            case COM_MULT_S:
                if (c == '*') {
                    fsm_state = COM_HALF_END_S;
                }
                break;
            case COM_HALF_END_S:
                switch (c) {
                    case '/':
                        fsm_state = COM_END_S;
                        break;
                    case '*':
                        break;
                    default:
                        fsm_state = COM_MULT_S;
                }
                break;
            case COM_END_S:
                fsm_state = START_S;
                break;

        }
        prev = c;
    }
    return t_array;
}