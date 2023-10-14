#include "scanner.h"
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "lists.h"

#define SUCCESS 0
#define LEX_INTERNAL_ERROR 1
#define LEX_ERROR 2

char *tokens_as_str[50] = {
        "TOKEN_IDENTIFIER",
        "TOKEN_DOUBLE_TYPE",
        "TOKEN_ELSE",
        "TOKEN_FUNC",
        "TOKEN_IF",
        "TOKEN_INT_TYPE",
        "TOKEN_LET",
        "TOKEN_RETURN",
        "TOKEN_STRING_TYPE",
        "TOKEN_VAR",
        "TOKEN_WHILE",
        "TOKEN_BOOL_TYPE",
        "TOKEN_FOR",
        "TOKEN_IN",
        "TOKEN_BREAK",
        "TOKEN_CONTINUE",
        "TOKEN_UNDERSCORE",
        "TOKEN_ASSIGNMENT",
        "TOKEN_ADDITION",
        "TOKEN_SUBTRACTION",
        "TOKEN_MULTIPLICATION",
        "TOKEN_DIVISION",
        "TOKEN_LESS_THAN",
        "TOKEN_LESS_THAN_OR_EQUAL_TO",
        "TOKEN_GREATER_THAN",
        "TOKEN_GREATER_THAN_OR_EQUAL_TO",
        "TOKEN_EQUAL_TO",
        "TOKEN_NOT_EQUAL_TO",
//        "TOKEN_NILABLE",
        "TOKEN_IS_NIL",
        "TOKEN_UNWRAP_NILABLE",
        "TOKEN_LOGICAL_AND",
        "TOKEN_LOGICAL_OR",
        "TOKEN_LOGICAL_NOT",
        "TOKEN_CLOSED_RANGE",
        "TOKEN_HALF_OPEN_RANGE",
        "TOKEN_INTEGER_LITERAL",
        "TOKEN_REAL_LITERAL",
        "TOKEN_STRING_LITERAL",
        "TOKEN_NIL_LITERAL",
        "TOKEN_TRUE_LITERAL",
        "TOKEN_FALSE_LITERAL",
        "TOKEN_LEFT_BRACKET",
        "TOKEN_RIGHT_BRACKET",
        "TOKEN_LEFT_BRACE",
        "TOKEN_RIGHT_BRACE",
        "TOKEN_COMMA",
        "TOKEN_COLON",
        "TOKEN_SEMICOLON",
        "TOKEN_ARROW"
};

char control_char = ' ';

token_array_t *tokens = NULL;

static int is_keyword(string_t *lexeme) {
    if str_eq_cstr(lexeme, "Double") return TOKEN_DOUBLE_TYPE;
    if str_eq_cstr(lexeme, "else") return TOKEN_ELSE;
    if str_eq_cstr(lexeme, "func") return TOKEN_FUNC;
    if str_eq_cstr(lexeme, "if") return TOKEN_IF;
    if str_eq_cstr(lexeme, "Int") return TOKEN_INT_TYPE;
    if str_eq_cstr(lexeme, "let") return TOKEN_LET;
    if str_eq_cstr(lexeme, "return") return TOKEN_RETURN;
    if str_eq_cstr(lexeme, "String") return TOKEN_STRING_TYPE;
    if str_eq_cstr(lexeme, "var") return TOKEN_VAR;
    if str_eq_cstr(lexeme, "while") return TOKEN_WHILE;
    if str_eq_cstr(lexeme, "Bool") return TOKEN_BOOL_TYPE;
    if str_eq_cstr(lexeme, "nil") return TOKEN_NIL_LITERAL;
    if str_eq_cstr(lexeme, "true") return TOKEN_TRUE_LITERAL;
    if str_eq_cstr(lexeme, "false") return TOKEN_FALSE_LITERAL;
    if str_eq_cstr(lexeme, "for") return TOKEN_FOR;
    if str_eq_cstr(lexeme, "in") return TOKEN_IN;
    if str_eq_cstr(lexeme, "break") return TOKEN_BREAK;
    if str_eq_cstr(lexeme, "continue") return TOKEN_CONTINUE;
    if str_eq_cstr(lexeme, "_") return TOKEN_UNDERSCORE;
    return -1;
}

static int verify_multiline_str(string_t *lexeme) {
    // split multiline lexeme by '\n' to lines
    two_way_list list;
    list_init(&list);
    string_t *copy = String.copy(lexeme);
    char *str = copy->str;
    char *token = strtok(str, "\n");
    while (token != NULL) {
        insert_last(&list, token);
        token = strtok(NULL, "\n");
    }
    char *first = list.first->data;
    list.first = list.first->next;
    char *second = list.first->data;
    list.first = list.first->next;
    char *last = list.last->data;
    list.last = list.last->prev;
    list.last->next = NULL;
    // go through lines and get minimum number of spaces in the beginning of each line, tab is one space
    int min_spaces = INT_MAX, spaces;
    element *tmp = list.first;
    while (tmp != NULL) {
        spaces = 0;
        while (((char *) tmp->data)[spaces] == ' ' || ((char *) tmp->data)[spaces] == '\t') {
            spaces++;
        }
        if (spaces < min_spaces) min_spaces = spaces;
        tmp = tmp->next;
    }
    // count number of spaces in the beginning of the last line
    spaces = 0;
    while (last[spaces] == ' ' || last[spaces] == '\t') {
        spaces++;
    }
    if (spaces > min_spaces) {
        return -1;
    }
    string_t *new_lexeme = String.ctor();
    if (new_lexeme == NULL) {
        return -1;
    }
    String.add_cstr(new_lexeme, first);
    String.add_char(new_lexeme, '\n');
    String.add_cstr(new_lexeme, second);
    String.add_char(new_lexeme, '\n');
    tmp = list.first;
    while (tmp != NULL) {
        String.add_cstr(new_lexeme, (char *) tmp->data + spaces);
        String.add_char(new_lexeme, '\n');
        tmp = tmp->next;
    }
    String.add_cstr(new_lexeme, "\"\"\"");
    String.assign(lexeme, new_lexeme);
    return 0;
}

static int add_token(token_type_t type, token_attribute attribute, bool has_attribute) {
    token_t *token = token_ctor(type, attribute, has_attribute);
    if (!isspace(control_char)) File.back_step();
    return token_array_add(token);
}

token_t *token_ctor(token_type_t type, token_attribute attribute, bool has_attribute) {
    token_t *token = malloc(sizeof(token_t));
    if (token == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        return NULL;
    }
    token->type = type;
    if (has_attribute) token->attribute = attribute;
    return token;
}

void token_dtor(token_t *token) {
    if (token == NULL) {
        return;
    }
    if (token->type == TOKEN_IDENTIFIER) {
        String.dtor(token->attribute.identifier);
    }
    free(token);
    token = NULL;
}

int token_array_ctor() {
    tokens = malloc(sizeof(token_array_t));
    if (tokens == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        return LEX_INTERNAL_ERROR;
    }
    tokens->array = malloc(sizeof(token_t *) * 16);
    if (tokens->array == NULL) {
        free(tokens);
        fprintf(stderr, "Error: malloc failed.\n");
        return LEX_INTERNAL_ERROR;
    }
    tokens->allocated = 16;
    tokens->length = 0;
    return SUCCESS;
}

void token_array_dtor() {
    if (tokens == NULL) {
        return;
    }
    for (int i = 0; i < tokens->length; i++) {
        token_dtor(tokens->array[i]);
    }
    free(tokens->array);
    free(tokens);
    tokens = NULL;
}

int token_array_add(token_t *token) {
    if (tokens == NULL) {
        fprintf(stderr, "Error: token array not initialized.\n");
        return LEX_INTERNAL_ERROR;
    }
    if (token == NULL) {
        fprintf(stderr, "Error: token is NULL.\n");
        return LEX_INTERNAL_ERROR;
    }
    if (tokens->length >= tokens->allocated) {
        void *tmp = realloc(tokens->array, sizeof(token_t *) * tokens->allocated * 2);
        if (tmp == NULL) {
            fprintf(stderr, "Error: realloc failed.\n");
            return LEX_INTERNAL_ERROR;
        }
        tokens->array = tmp;
        tokens->allocated *= 2;
    }
    tokens->array[tokens->length] = token;
    tokens->length++;
    return SUCCESS;
}

void token_print(token_t *token) {
    if (token == NULL) {
        return;
    }
    printf("Token: %32s", tokens_as_str[token->type]);
    if (token->type == TOKEN_IDENTIFIER) {
        printf(",   ID:   %s", token->attribute.identifier->str);
    } else if (token->type == TOKEN_INTEGER_LITERAL) {
        printf(",   INT:  %d", token->attribute.integer);
    } else if (token->type == TOKEN_REAL_LITERAL) {
        printf(",   REAL: %a", token->attribute.real);
    } else if (token->type == TOKEN_STRING_LITERAL) {
        printf(",   STR:  %s", token->attribute.string->str);
    }
    switch (token->type)
    {
        case TOKEN_BOOL_TYPE:
        case TOKEN_DOUBLE_TYPE:
        case TOKEN_INT_TYPE:
        case TOKEN_STRING_TYPE:
            if (token->attribute.nilable) printf(",   NILABLE");
            else printf(",   NON-NILABLE");
            break;
        default:
            break;
    }
    printf("\n");
}

int source_code_to_tokens() {
    // Initial values DO NOT CHANGE
    fsm_state_t fsm_state = START_S;
    bool multiline = false;
    string_t *lexeme = String.ctor();
    token_attribute attribute;
    token_type_t type;
    char c, prev = ' ', prev_prev = ' ', count = 0;
    int keyword_code;

    // FSM loop
    while ((c = File.getc()) != EOF) {
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
                        multiline = false;
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
                        break;
                    case '*':
                        fsm_state = MUL_S;
                        break;
                    case '!':
                        fsm_state = EXCL_MARK_S;
                        break;
                    case '?':
                        fsm_state = IS_NIL_1_S;
                        break;
                    case '>':
                        fsm_state = GREATER_S;
                        break;
                    case '<':
                        fsm_state = LESS_S;
                        break;
                    case '-':
                        fsm_state = MINUS_S;
                        break;
                    case '=':
                        fsm_state = ASSIGN_S;
                        break;
                    case '&':
                        fsm_state = LOGICAL_AND1_S;
                        break;
                    case '|':
                        fsm_state = LOGICAL_OR1_S;
                        break;
                        // Punctuators
                    case '{':
                        fsm_state = BRACE_L_S;
                        break;
                    case '}':
                        fsm_state = BRACE_R_S;
                        break;
                    case '(':
                        fsm_state = BRACKET_L_S;
                        break;
                    case ')':
                        fsm_state = BRACKET_R_S;
                        break;
                    case ':':
                        fsm_state = COLON_S;
                        break;
                    case ';':
                        fsm_state = SEMICOLON_S;
                        break;
                    case ',':
                        fsm_state = COMMA_S;
                        break;
                    case '.':
                        fsm_state = RANGE_START_S;
                        break;
                    case '/':
                        fsm_state = DIV_S;
                        break;
                    case '_':
                    case 'a' ... 'z':
                    case 'A' ... 'Z':
                        fsm_state = ID_S;
                        String.add_char(lexeme, c);
                        break;
                        // Other
                    default:
                        fprintf(stderr, "Error: Unknown character '%c' (0x%02x).\n", c, c);
                        return LEX_ERROR;
                }
                break;
            case PLUS_S:
                type = TOKEN_ADDITION;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case MUL_S:
                type = TOKEN_MULTIPLICATION;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case EXCL_MARK_S:
                if (c == '=') {
                    fsm_state = NOT_EQUAL_S;
                } else {
                    if (isspace(prev_prev) || (isalnum(c) || c == '_')) type = TOKEN_LOGICAL_NOT;
                    else type = TOKEN_UNWRAP_NILABLE;
                    add_token(type, attribute, false);
                    fsm_state = START_S;
                }
                break;
            case NOT_EQUAL_S:
                type = TOKEN_NOT_EQUAL_TO;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case IS_NIL_1_S:
                if (c == '?') {
                    fsm_state = IS_NIL_2_S;
                } else {
                    fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected '%c' (0x%02x)", c, c, '?', '?');
                    return LEX_ERROR;
                }
                break;
            case IS_NIL_2_S:
                type = TOKEN_IS_NIL;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case GREATER_S:
                if (c == '=') {
                    fsm_state = GREATER_EQUAL_S;
                } else {
                    type = TOKEN_GREATER_THAN;
                    add_token(type, attribute, false);
                    fsm_state = START_S;
                }
                break;
            case GREATER_EQUAL_S:
                type = TOKEN_GREATER_THAN_OR_EQUAL_TO;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case LESS_S:
                if (c == '=') {
                    fsm_state = LESS_EQUAL_S;
                } else {
                    type = TOKEN_LESS_THAN;
                    add_token(type, attribute, false);
                    fsm_state = START_S;
                }
                break;
            case LESS_EQUAL_S:
                type = TOKEN_LESS_THAN_OR_EQUAL_TO;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case MINUS_S:
                if (c == '>') {
                    fsm_state = ARROW_S;
                } else {
                    type = TOKEN_SUBTRACTION;
                    add_token(type, attribute, false);
                    fsm_state = START_S;
                }
                break;
            case ARROW_S:
                type = TOKEN_ARROW;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case ASSIGN_S:
                if (c == '=') {
                    fsm_state = EQUAL_S;

                } else {
                    type = TOKEN_ASSIGNMENT;
                    add_token(type, attribute, false);
                    fsm_state = START_S;
                }
                break;
            case EQUAL_S:
                type = TOKEN_EQUAL_TO;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case BRACE_L_S:
                type = TOKEN_LEFT_BRACE;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case BRACE_R_S:
                type = TOKEN_RIGHT_BRACE;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case BRACKET_L_S:
                type = TOKEN_LEFT_BRACKET;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case BRACKET_R_S:
                type = TOKEN_RIGHT_BRACKET;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case COLON_S:
                type = TOKEN_COLON;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case SEMICOLON_S:
                type = TOKEN_SEMICOLON;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case COMMA_S:
                type = TOKEN_COMMA;
                add_token(type, attribute, false);
                fsm_state = START_S;
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
                        type = TOKEN_INTEGER_LITERAL;
                        attribute.integer = strtol(lexeme->str, NULL, 10);
                        add_token(type, attribute, true);
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
                    case '?':
                        fsm_state = TYPE_NIL_S;
                        keyword_code = is_keyword(lexeme);
                        if (keyword_code == -1) {
                            fprintf(stderr, "Expected data type (Int, Double, Bool, String). Got: %s\n", lexeme->str);
                            return LEX_ERROR;
                        }
                        String.add_char(lexeme, c);
                        break;
                    default:
                        keyword_code = is_keyword(lexeme);
                        if (keyword_code == -1) {
                            type = TOKEN_IDENTIFIER;
                            attribute.identifier = String.copy(lexeme);
                            add_token(type, attribute, true);
                        } else {
                            type = keyword_code;
                            attribute.nilable = false;
                            add_token(type, attribute, true);
                        }
                        fsm_state = START_S;
                        String.clear(lexeme);
                }
                break;
            case TYPE_NIL_S:
                type = keyword_code;
                attribute.nilable = true;
                add_token(type, attribute, true);
                fsm_state = START_S;
                String.clear(lexeme);
                break;
            case REAL_S:
                if ('0' <= c && c <= '9') {
                    fsm_state = REAL_NUM_S;
                    String.add_char(lexeme, c);
                } else {
                    fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected decimal digit", c, c);
                    return LEX_ERROR;
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
                        type = TOKEN_REAL_LITERAL;
                        attribute.real = strtod(lexeme->str, NULL);
                        add_token(type, attribute, true);
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
                        fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected decimal digit or '+' or '-'", c, c);
                        return LEX_ERROR;
                }
                break;
            case EXP_SIGN_S:
                if ('0' <= c && c <= '9') {
                    fsm_state = REAL_EXP_S;
                    String.add_char(lexeme, c);
                } else {
                    fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected decimal digit", c, c);
                    return LEX_ERROR;
                }
                break;
            case REAL_EXP_S:
                if ('0' <= c && c <= '9') {
                    String.add_char(lexeme, c);
                } else {
                    type = TOKEN_REAL_LITERAL;
                    attribute.real = strtod(lexeme->str, NULL);
                    add_token(type, attribute, true);
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
                    case 32 ... 33:
                    case 35 ... 91:
                    case 93 ... 126:
                        String.add_char(lexeme, c);
                        break;
                    default:
                        fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected printable character", c, c);
                        return LEX_ERROR;
                }
                break;
            case STR_1_END_S:
                if (c == '"') {
                    fsm_state = STR_2_START_S;
                    String.add_char(lexeme, c);
                } else {
                    type = TOKEN_STRING_LITERAL;
                    attribute.string = String.copy(lexeme);
                    add_token(type, attribute, true);
                    fsm_state = START_S;
                    String.clear(lexeme);
                }
                break;
            case STR_2_START_S:
                if (c == '\n') {
                    fsm_state = STR_MULT_HALF_S;
                    multiline = true;
                    String.add_char(lexeme, c);
                } else {
                    fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected newline", c, c);
                    return LEX_ERROR;
                }
                break;
            case STR_MULT_HALF_S:
                if ((9 <= c && c <= 13) || c == 32 || c == 0) {
                    fsm_state = STR_MULT_HALF_S;
                } else if (c > 31) {
                    fsm_state = STR_MULT_S;
                    String.add_char(lexeme, c);
                } else {
                    fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected printable character", c, c);
                    return LEX_ERROR;
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
                    case 9:
                    case 11 ... 13:
                    case 32 ... 91:
                    case 93 ... 126:
                        String.add_char(lexeme, c);
                        break;
                    default:
                        return LEX_ERROR;
                }
                break;
            case STR_LF_S:
                switch (c) {
                    case '"':
                        String.add_char(lexeme, c);
                        fsm_state = STR_2_E_S;
                        break;
                    case 9 ... 13:
                    case 32:
                        String.add_char(lexeme, c);
                        fsm_state = STR_LF_S;
                        break;
                    case 33:
                    case 35 ... 126:
                        String.add_char(lexeme, c);
                        fsm_state = STR_MULT_S;
                        break;
                    default:
                        return LEX_ERROR;
                }
                break;
            case STR_2_E_S:
                switch (c) {
                    case '"':
                        String.add_char(lexeme, c);
                        fsm_state = STR_2_EE_S;
                        break;
                    case 32 ... 33:
                    case 35 ... 126:
                        String.add_char(lexeme, c);
                        fsm_state = STR_MULT_S;
                        break;
                    default:
                        return LEX_ERROR;
                }
                break;
            case STR_2_EE_S:
                switch (c) {
                    case '"':
                        String.add_char(lexeme, c);
                        fsm_state = STR_2_END_S;
                        break;
                    case 32 ... 33:
                    case 35 ... 126:
                        String.add_char(lexeme, c);
                        fsm_state = STR_MULT_S;
                        break;
                    default:
                        return LEX_ERROR;
                }
                break;
            case STR_2_END_S:
                type = TOKEN_STRING_LITERAL;
                attribute.string = String.copy(lexeme);
                if (verify_multiline_str(attribute.string)) return LEX_ERROR;
                add_token(type, attribute, true);
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
                        return LEX_ERROR;
                }
                break;
            case U_SEC_S:
                if (c == '{') {
                    String.add_char(lexeme, c);
                    fsm_state = U_SEC_START_S;
                } else {
                    return LEX_ERROR;
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
                        return LEX_ERROR;
                }
                break;
            case U_SEC_NUM_S:
                if (count >= 8) {
                    return LEX_ERROR;
                }
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
                        return LEX_ERROR;
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
                        type = TOKEN_DIVISION;
                        add_token(type, attribute, false);
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
                if (!isspace(control_char)) File.back_step();
                c = ' ';
                fsm_state = START_S;
                break;
            case LOGICAL_AND1_S:
                if (c == '&') {
                    fsm_state = LOGICAL_AND2_S;
                } else {
                    return LEX_ERROR;
                }
                break;
            case LOGICAL_AND2_S:
                type = TOKEN_LOGICAL_AND;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case LOGICAL_OR1_S:
                if (c == '|')
                    fsm_state = LOGICAL_OR2_S;
                else
                    return LEX_ERROR;
                break;
            case LOGICAL_OR2_S:
                type = TOKEN_LOGICAL_OR;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case RANGE_START_S:
                if (c == '.')
                    fsm_state = RANGE_CLOSED_S;
                else
                    return LEX_ERROR;
                break;
            case RANGE_CLOSED_S:
                if (c == '<')
                    fsm_state = RANGE_HALF_OPEN_S;
                else {
                    type = TOKEN_CLOSED_RANGE;
                    add_token(type, attribute, false);
                    fsm_state = START_S;
                }
                break;
            case RANGE_HALF_OPEN_S:
                type = TOKEN_HALF_OPEN_RANGE;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
        }
        prev_prev = prev;
        prev = c;
    }
    return SUCCESS;
}