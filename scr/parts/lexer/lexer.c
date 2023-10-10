#include "lexer.h"
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>


static char control_char = ' ';

static file_t *curr_file = NULL;

static int is_keyword(string_t *lexeme) {
    if str_eq_cstr(lexeme, "Double") return DOUBLE_TYPE;
    if str_eq_cstr(lexeme, "else") return ELSE;
    if str_eq_cstr(lexeme, "func") return FUNC;
    if str_eq_cstr(lexeme, "if") return IF;
    if str_eq_cstr(lexeme, "Int") return INT_TYPE;
    if str_eq_cstr(lexeme, "let") return LET;
    if str_eq_cstr(lexeme, "return") return RETURN;
    if str_eq_cstr(lexeme, "String") return STRING_TYPE;
    if str_eq_cstr(lexeme, "var") return VAR;
    if str_eq_cstr(lexeme, "while") return WHILE;
    if str_eq_cstr(lexeme, "Bool") return BOOL_TYPE;
    if str_eq_cstr(lexeme, "nil") return NIL_LITERAL;
    if str_eq_cstr(lexeme, "true") return TRUE_LITERAL;
    if str_eq_cstr(lexeme, "false") return FALSE_LITERAL;
    return -1;
}

static int add_token(token_array_t *token_array, token_type_t type, token_subtype_t subtype, string_t *lexeme) {
    token_t *token = token_ctor(type, subtype, lexeme);
    if (!isspace(control_char)) File.back_step(curr_file);
    return token_array_add(token_array, token);
}

token_t *token_ctor(token_type_t type, token_subtype_t subtype, string_t *lexeme) {
    token_t *token = malloc(sizeof(token_t));
    if (token == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        return NULL;
    }
    token->type = type;
    token->subtype = subtype;
    token->lexeme = String.copy(lexeme);
    return token;
}

void token_dtor(token_t *token) {
    if (token == NULL) {
        return;
    }
    String.dtor(token->lexeme);
    free(token);
    token = NULL;
}

token_array_t *token_array_ctor() {
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

void token_print(token_t *token) {
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

token_array_t *source_code_to_tokens(file_t *file) {

    // Initial values DO NOT CHANGE
    curr_file = file;
    fsm_state_t fsm_state = START_S;
    token_array_t *t_array = token_array_ctor();
    bool multiline = false;
    string_t *lexeme = String.ctor();
    token_subtype_t subtype;
    token_type_t type;
    char c, prev = ' ', prev_prev = ' ', count = 0;
    int keyword_code;

    // FSM loop
    while ((c = File.getc(file)) != EOF) {
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
                        String.add_char(lexeme, c);
                        break;
                        // Number literals
                    case '0' ... '9':
                        fsm_state = INTEGER_S;
                        String.add_char(lexeme, c);
                        break;
                        // Operators
                    case '+':
                        fsm_state = PLUS_S;
                        String.add_char(lexeme, c);
                        break;
                    case '*':
                        fsm_state = MUL_S;
                        String.add_char(lexeme, c);
                        break;
                    case '!':
                        fsm_state = EXCL_MARK_S;
                        String.add_char(lexeme, c);
                        break;
                    case '?':
                        fsm_state = NILABLE_S;
                        String.add_char(lexeme, c);
                        break;
                    case '>':
                        fsm_state = GREATER_S;
                        String.add_char(lexeme, c);
                        break;
                    case '<':
                        fsm_state = LESS_S;
                        String.add_char(lexeme, c);
                        break;
                    case '-':
                        fsm_state = MINUS_S;
                        String.add_char(lexeme, c);
                        break;
                    case '=':
                        fsm_state = ASSIGN_S;
                        String.add_char(lexeme, c);
                        break;
                    case '&':
                        fsm_state = LOGICAL_AND1_S;
                        String.add_char(lexeme, c);
                        break;
                    case '|':
                        fsm_state = LOGICAL_OR1_S;
                        String.add_char(lexeme, c);
                        break;
                        // Punctuators
                    case '{':
                        fsm_state = BRACE_L_S;
                        String.add_char(lexeme, c);
                        break;
                    case '}':
                        fsm_state = BRACE_R_S;
                        String.add_char(lexeme, c);
                        break;
                    case '(':
                        fsm_state = BRACKET_L_S;
                        String.add_char(lexeme, c);
                        break;
                    case ')':
                        fsm_state = BRACKET_R_S;
                        String.add_char(lexeme, c);
                        break;
                    case ':':
                        fsm_state = COLON_S;
                        String.add_char(lexeme, c);
                        break;
                    case ';':
                        fsm_state = SEMICOLON_S;
                        String.add_char(lexeme, c);
                        break;
                    case ',':
                        fsm_state = COMMA_S;
                        String.add_char(lexeme, c);
                        break;
                    case '/':
                        fsm_state = DIV_S;
                        String.add_char(lexeme, c);
                        break;
                    case '_':
                    case 'a' ... 'z':
                    case 'A' ... 'Z':
                        fsm_state = ID_S;
                        String.add_char(lexeme, c);
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
                String.clear(lexeme);
                break;
            case MUL_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = MULTIPLICATION;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case EXCL_MARK_S:
                if (c == '=') {
                    fsm_state = NOT_EQUAL_S;
                    String.add_char(lexeme, c);
                } else {
                    type = TOKEN_OPERATOR;
                    if (isspace(prev_prev)) subtype.operator_type = LOGICAL_NOT;
                    else subtype.operator_type = UNWRAP_NILABLE;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    String.clear(lexeme);
                }
                break;
            case NOT_EQUAL_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = NOT_EQUAL_TO;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case NILABLE_S:
                if (c == '?') {
                    fsm_state = IS_NIL_S;
                    String.add_char(lexeme, c);
                } else {
                    type = TOKEN_OPERATOR;
                    subtype.operator_type = NILABLE;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    String.clear(lexeme);
                }
                break;
            case IS_NIL_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = IS_NIL;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case GREATER_S:
                if (c == '=') {
                    fsm_state = GREATER_EQUAL_S;
                    String.add_char(lexeme, c);
                } else {
                    type = TOKEN_OPERATOR;
                    subtype.operator_type = GREATER_THAN;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    String.clear(lexeme);
                }
                break;
            case GREATER_EQUAL_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = GREATER_THAN_OR_EQUAL_TO;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case LESS_S:
                if (c == '=') {
                    fsm_state = LESS_EQUAL_S;
                    String.add_char(lexeme, c);
                } else {
                    type = TOKEN_OPERATOR;
                    subtype.operator_type = LESS_THAN;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    String.clear(lexeme);
                }
                break;
            case LESS_EQUAL_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = LESS_THAN_OR_EQUAL_TO;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case MINUS_S:
                if (c == '>') {
                    fsm_state = ARROW_S;
                    String.add_char(lexeme, c);
                } else {
                    type = TOKEN_OPERATOR;
                    subtype.operator_type = SUBTRACTION;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    String.clear(lexeme);
                }
                break;
            case ARROW_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = ARROW;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case ASSIGN_S:
                if (c == '=') {
                    fsm_state = EQUAL_S;
                    String.add_char(lexeme, c);
                } else {
                    type = TOKEN_OPERATOR;
                    subtype.operator_type = ASSIGNMENT;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    String.clear(lexeme);
                }
                break;
            case EQUAL_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = EQUAL_TO;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case BRACE_L_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = LEFT_BRACE;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case BRACE_R_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = RIGHT_BRACE;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case BRACKET_L_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = LEFT_BRACKET;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case BRACKET_R_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = RIGHT_BRACKET;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case COLON_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = COLON;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case SEMICOLON_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = SEMICOLON;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case COMMA_S:
                type = TOKEN_PUNCTUATOR;
                subtype.punctuator_type = COMMA;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case INTEGER_S:
                switch (c) {
                    case '0' ... '9':
                        String.add_char(lexeme, c);
                        break;
                    case 'e':
                    case 'E':
                        fsm_state = REAL_E_S;
                        String.add_char(lexeme, c);
                        break;
                    case '.':
                        fsm_state = REAL_S;
                        String.add_char(lexeme, c);
                        break;
                    default:
                        type = TOKEN_LITERAL;
                        subtype.literal_type = INTEGER_LITERAL;
                        add_token(t_array, type, subtype, lexeme);
                        fsm_state = START_S;
                        String.clear(lexeme);
                }
                break;
            case ID_S:
                switch (c) {
                    case '_':
                    case 'a' ... 'z':
                    case 'A' ... 'Z':
                    case '0' ... '9':
                        String.add_char(lexeme, c);
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
                            case TRUE_LITERAL:
                                type = TOKEN_LITERAL;
                                subtype.literal_type = TRUE_LITERAL;
                                break;
                            case FALSE_LITERAL:
                                type = TOKEN_LITERAL;
                                subtype.literal_type = FALSE_LITERAL;
                                break;
                            default:
                                type = TOKEN_KEYWORD;
                                subtype.keyword_type = keyword_code;
                        }
                        add_token(t_array, type, subtype, lexeme);
                        fsm_state = START_S;
                        String.clear(lexeme);
                }
                break;
            case REAL_S:
                if ('0' <= c && c <= '9') {
                    fsm_state = REAL_NUM_S;
                    String.add_char(lexeme, c);
                } else {
                    return NULL;
                }
                break;
            case REAL_NUM_S:
                switch (c) {
                    case '0' ... '9':
                        String.add_char(lexeme, c);
                        break;
                    case 'e':
                    case 'E':
                        fsm_state = REAL_E_S;
                        String.add_char(lexeme, c);
                        break;
                    default:
                        type = TOKEN_LITERAL;
                        subtype.literal_type = REAL_LITERAL;
                        add_token(t_array, type, subtype, lexeme);
                        fsm_state = START_S;
                        String.clear(lexeme);
                }
                break;
            case REAL_E_S:
                switch (c) {
                    case '0' ... '9':
                        fsm_state = REAL_EXP_S;
                        String.add_char(lexeme, c);
                        break;
                    case '+':
                    case '-':
                        fsm_state = EXP_SIGN_S;
                        String.add_char(lexeme, c);
                        break;
                    default:
                        return NULL;
                }
                break;
            case EXP_SIGN_S:
                if ('0' <= c && c <= '9') {
                    fsm_state = REAL_EXP_S;
                    String.add_char(lexeme, c);
                } else {
                    return NULL;
                }
                break;
            case REAL_EXP_S:
                if ('0' <= c && c <= '9') {
                    String.add_char(lexeme, c);
                } else {
                    type = TOKEN_LITERAL;
                    subtype.literal_type = REAL_LITERAL;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    String.clear(lexeme);
                }
                break;
                // string literals
            case STR_START_S:
                switch (c) {
                    case '"':
                        fsm_state = STR_1_END_S;
                        String.add_char(lexeme, c);
                        break;
                    case '\\':
                        fsm_state = ESC_S;
                        String.add_char(lexeme, c);
                        break;
                    case 31 ... 33:
                    case 35 ... 91:
                    case 93 ... 126:
                        String.add_char(lexeme, c);
                        break;
                    default:
                        return NULL;
                }
                break;
            case STR_1_END_S:
                if (c == '"') {
                    fsm_state = STR_2_START_S;
                    String.add_char(lexeme, c);
                } else {
                    type = TOKEN_LITERAL;
                    subtype.literal_type = STRING_LITERAL;
                    add_token(t_array, type, subtype, lexeme);
                    fsm_state = START_S;
                    String.clear(lexeme);
                }
                break;
            case STR_2_START_S:
                if (c == '\n') {
                    fsm_state = STR_MULT_S;
                    multiline = true;
                    String.add_char(lexeme, c);
                } else {
                    return NULL;
                }
                break;
            case STR_MULT_S:
                switch (c) {
                    case '\n':
                        String.add_char(lexeme, c);
                        fsm_state = STR_LF_S;
                        break;
                    case '\\':
                        String.add_char(lexeme, c);
                        fsm_state = ESC_S;
                        break;
                    case 31 ... 91:
                    case 93 ... 126:
                        String.add_char(lexeme, c);
                        break;
                    default:
                        return NULL;
                }
                break;
            case STR_LF_S:
                switch (c) {
                    case '"':
                        String.add_char(lexeme, c);
                        fsm_state = STR_2_E_S;
                        break;
                    case 31 ... 33:
                    case 35 ... 126:
                        String.add_char(lexeme, c);
                        fsm_state = STR_MULT_S;
                        break;
                    default:
                        return NULL;
                }
                break;
            case STR_2_E_S:
                switch (c) {
                    case '"':
                        String.add_char(lexeme, c);
                        fsm_state = STR_2_EE_S;
                        break;
                    case 31 ... 33:
                    case 35 ... 126:
                        String.add_char(lexeme, c);
                        fsm_state = STR_MULT_S;
                        break;
                    default:
                        return NULL;
                }
                break;
            case STR_2_EE_S:
                switch (c) {
                    case '"':
                        String.add_char(lexeme, c);
                        fsm_state = STR_2_END_S;
                        break;
                    case 31 ... 33:
                    case 35 ... 126:
                        String.add_char(lexeme, c);
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
                String.clear(lexeme);
                break;
            case ESC_S:
                switch (c) {
                    case '"':
                    case 'n':
                    case 'r':
                    case 't':
                    case '\\':
                        String.add_char(lexeme, c);
                        fsm_state = multiline ? STR_MULT_S : STR_START_S;
                        break;
                    case 'u':
                        String.add_char(lexeme, c);
                        fsm_state = U_SEC_S;
                        break;
                    case '(':  // TODO interpolation
                    default:
                        return NULL;
                }
                break;
            case U_SEC_S:
                if (c == '{') {
                    String.add_char(lexeme, c);
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
                        String.add_char(lexeme, c);
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
                        String.add_char(lexeme, c);
                        fsm_state = U_SEC_NUM_S;
                        count++;
                        break;
                    case '}':
                        String.add_char(lexeme, c);
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
                        String.clear(lexeme);
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
            case LOGICAL_AND1_S:
                if (c == '&') {
                    fsm_state = LOGICAL_AND2_S;
                    String.add_char(lexeme, c);
                } else {
                    return NULL;
                }
                break;
            case LOGICAL_AND2_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = LOGICAL_AND;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case LOGICAL_OR1_S:
                if (c == '|') {
                    fsm_state = LOGICAL_OR2_S;
                    String.add_char(lexeme, c);
                } else {
                    return NULL;
                }
                break;
            case LOGICAL_OR2_S:
                type = TOKEN_OPERATOR;
                subtype.operator_type = LOGICAL_OR;
                add_token(t_array, type, subtype, lexeme);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
        }
        prev_prev = prev;
        prev = c;
    }
    return t_array;
}