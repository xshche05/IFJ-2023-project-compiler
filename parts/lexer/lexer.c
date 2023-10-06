#include "lexer.h"
#include <stdlib.h>
#include <stdbool.h>


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
    if (token_array->length == token_array->allocated) {
        void *tmp = realloc(token_array->array, sizeof(token_t *) * token_array->allocated * 3/2);
        if (tmp == NULL) {
            return -1;
        }
        token_array->array = tmp;
        token_array->allocated *= 3/2;
    }
    token_array->array[token_array->length] = token;
    token_array->length++;
    return 0;
}

int add_token(token_array_t *token_array, token_type_t type, token_subtype_t subtype, string_t *lexeme) {
    token_t *token = token_init(type, subtype, lexeme);
    return token_array_add(token_array, token);
}

token_array_t *source_code_to_tokens(file_t *file) {
    // Initial values DO NOT CHANGE
    fsm_state_t fsm_state = START_S;
    token_array_t *t_array = token_array_init();
    bool multiline = false;
    string_t *lexeme = string_init();
    token_subtype_t subtype;
    token_type_t type;
    char c, prev, count = 0;
    // FSM loop
    while ((c = file_getc(file)) != EOF) {
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
                    // Other
                    default:
                        return NULL;    
                }
                break;
            case WHITESYMB_S:
                break;
            case PLUS_S:
                break;
            case MUL_S:
                break;
            case EXCL_MARK_S:
                break;
            case NOT_EQUAL_S:
                break;
            case NILABLE_S:
                break;
            case IS_NIL_S:
                break;
            case GREATER_S:
                break;
            case GREATER_EQUAL_S:
                break;
            case LESS_S:
                break;
            case LESS_EQUAL_S:
                break;
            case MINUS_S:
                break;
            case ARROW_S:
                break;
            case ASSIGN_S:
                break;
            case EQUAL_S:
                break;
            case BRACE_L_S:
                break;
            case BRACE_R_S:
                break;
            case BRACKET_L_S:
                break;
            case BRACKET_R_S:
                break;
            case COLON_S:
                break;
            case SEMICOLON_S:
                break;
            case COMMA_S:
                break;
            case INTEGER_S:
                break;
            case ID_S:
                break;
            case REAL_S:
                break;
            case REAL_NUM_S:
                break;
            case REAL_E_S:
                break;
            case REAL_EXP_S:
                break;
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
                switch (c) {
                    case '"':
                        fsm_state = STR_2_START_S;
                        string_add_char(lexeme, c);
                        break;
                    default:
                        type = TOKEN_LITERAL;
                        subtype.literal_type = STRING_LITERAL;
                        add_token(t_array, type, subtype, lexeme);
                        fsm_state = START_S;
                        string_clear(lexeme);
                }
                break;
            case STR_2_START_S:
                switch (c) {
                    case '\n':
                        fsm_state = STR_MULT_S;
                        multiline = true;
                        string_add_char(lexeme, c);
                        break;
                    default:
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
                switch (c) {
                    case '{':
                        string_add_char(lexeme, c);
                        fsm_state = U_SEC_START_S;
                        break;
                    default:
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
            case DIV_S:
                break;
            case COM_SINGL_S:
                break;
            case COM_MULT_S:
                break;
            case COM_HALF_END_S:
                break;
            case COM_END_S:
                break;

        }
        prev = c;
    }
    return t_array;
}