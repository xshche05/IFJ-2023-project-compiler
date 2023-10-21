#include "scanner.h"
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "lists.h"
#include "token/token.h"

char control_char = ' ';

token_array_t *tokens = NULL;
bool token_added = false;

#define ERROR(stream, msg, ...) fprintf(stream, "Error: " msg "\n\n", ##__VA_ARGS__); \
fprintf(stderr, "%s\n", File.print_line());                                          \
fprintf(stderr, "%*c\n\n", File.column()+1, '^');                                     \
fprintf(stderr, "Line:   %d\nColumn: %d\n\n\n", File.line()+1, File.column()+1);                                         \


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

static string_t *verify_str(string_t *lexeme, bool multiline) {
    // if multiline string, split by '\n'
    string_t *tmp_lex = String.copy(lexeme);
    string_t *new_lex = String.ctor();
    if (multiline) {
        dynamic_array_t *lines = DynamicArray.ctor();
        char *line = strtok(tmp_lex->str, "\n");
        while (line != NULL) {
            char *tmp = malloc(sizeof(char) * (strlen(line) + 1));
            strcpy(tmp, line);
            DynamicArray.add(lines, tmp);
            line = strtok(NULL, "\n");
        }
        int spaces_before_closing_quotes = 0;
        while (isspace(*((char *) DynamicArray.get(lines, lines->size - 1) + spaces_before_closing_quotes))) {
            spaces_before_closing_quotes++;
        }
        int number_of_spaces_in_first_line = 0;
        while (isspace(*((char *) DynamicArray.get(lines, 1) + number_of_spaces_in_first_line))) {
            number_of_spaces_in_first_line++;
        }
        if (number_of_spaces_in_first_line < spaces_before_closing_quotes) {
            fprintf(stderr, "Error: Invalid multiline string. Before closing quotes too many whitespaces\n");
            String.dtor(tmp_lex);
            String.dtor(new_lex);
            return NULL;
        } else if (number_of_spaces_in_first_line)
        {
            char *new_first = (char*) DynamicArray.get(lines, 1) + number_of_spaces_in_first_line;
            String.add_cstr(new_lex, new_first);
        } else {
            String.add_cstr(new_lex, DynamicArray.get(lines, 1));
        }
        if (lines->size > 3) String.add_char(new_lex, '\n');
        if (spaces_before_closing_quotes > 0) {
            for (int i = 2; i < lines->size - 1; i++) {
                int spaces_to_remove = spaces_before_closing_quotes;
                char *cur_line = DynamicArray.get(lines, i) + spaces_to_remove;
                if (!isspace(*(cur_line-1))) {
                    fprintf(stderr, "Error: Invalid multiline string. Before closing quotes too many whitespaces\n");
                    String.dtor(tmp_lex);
                    String.dtor(new_lex);
                    return NULL;
                }
                String.add_cstr(new_lex, cur_line);
                if (i != lines->size - 2) String.add_char(new_lex, '\n');
            }
        }
        else {
            for (int i = 2; i < lines->size-1; i++) {
                String.add_cstr(new_lex, DynamicArray.get(lines, i));
                if (i != lines->size - 2) String.add_char(new_lex, '\n');
            }
        }
        DynamicArray.dtor(lines);
    }
    else {
        for (int i = 1; i < tmp_lex->length - 1; i++) {
            String.add_char(new_lex, tmp_lex->str[i]);
        }
    }
    String.clear(tmp_lex);
    // replace all escape sequences in new_lex to form "\ddd", where ddd is decimal ascii code
    char *tmp = new_lex->str;
    for (int i = 0; i < new_lex->length; i++) {
        if (tmp[i] == '\\') {
            if (tmp[i+1] != 'u') {
                char *c;
                if (i + 1 >= new_lex->length) {
                    fprintf(stderr, "Error: Invalid string literal. Backslash at the end of string\n");
                    String.dtor(tmp_lex);
                    String.dtor(new_lex);
                    return NULL;
                }
                switch (tmp[i + 1]) {
                    case 'n':
                        c = "\\010";
                        break;
                    case 't':
                        c = "\\009";
                        break;
                    case 'r':
                        c = "\\013";
                        break;
                    case '\\':
                        c = "\\092";
                        break;
                    case '"':
                        c = "\\034";
                        break;
                    default:
                        fprintf(stderr, "Error: Invalid string literal. Unknown escape sequence '\\%c'\n", tmp[i + 1]);
                        String.dtor(tmp_lex);
                        String.dtor(new_lex);
                        return NULL;
                }
                String.add_cstr(tmp_lex, c);
                i++;
            }
            else {
                i+=3;
                char num[9];
                int number;
                // while dont get '}' uppend chars to num
                int j = 0;
                while (tmp[i] != '}') {
                    if (j > 7) {
                        fprintf(stderr, "Error: Invalid string literal. Unicode escape sequence too long\n");
                        String.dtor(tmp_lex);
                        String.dtor(new_lex);
                        return NULL;
                    }
                    num[j] = tmp[i];
                    i++;
                    j++;
                }
                num[j] = '\0';
                number = (int) strtol(num, NULL, 16);
                if (number < 256) {
                    char c[20];
                    sprintf(c, "\\%03d", number);
                    String.add_cstr(tmp_lex, c);
                }
            }
        }
        else if ((0 <= tmp[i] && tmp[i] <= 32) || tmp[i] == 35 || tmp[i] == 92) {
            char c[20];
            sprintf(c, "\\%03d", tmp[i]);
            String.add_cstr(tmp_lex, c);
        }
        else {
            String.add_char(tmp_lex, tmp[i]);
        }
    }
    String.dtor(new_lex);
    return tmp_lex;
}

static int add_token(token_type_t type, token_attribute attribute, bool has_attribute) {
    token_t *token = Token.ctor(type, attribute, has_attribute);
    if (!isspace(control_char) && type != TOKEN_NEWLINE) File.back_step();
    if (type == TOKEN_NEWLINE) token_added = false;
    else token_added = true;
    return TokenArray.add(token);
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
    int comment_cnt = 0;

    // FSM loop
    while ((c = File.getc()) != EOF) {
        control_char = c;
        switch (fsm_state) {
            case START_S:
                if (prev == '\n' && token_added) {
                    add_token(TOKEN_NEWLINE, attribute, false);
                }
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
                        //fprintf(stderr, "Error: Unknown character '%c' (0x%02x).\n", c, c);
                        ERROR(stderr, "Unknown character '%c' (0x%02x)", c, c)
                        return LEXICAL_ERROR;
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
                    //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected '%c' (0x%02x)\n", c, c, '?', '?');
                    ERROR(stderr, "Unknown character '%c' (0x%02x). Expected '%c' (0x%02x)", c, c, '?', '?')
                    return LEXICAL_ERROR;
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
                        attribute.integer = (int)strtol(lexeme->str, NULL, 10);
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
                            ERROR(stderr, "Unknown type '%s', expected 'Int', 'Double', 'Bool' or 'String' types before", lexeme->str)
                            return LEXICAL_ERROR;
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
                    //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected decimal digit\n", c, c);
                    ERROR(stderr, "Unknown character '%c' (0x%02x). Expected decimal digit", c, c)
                    return LEXICAL_ERROR;
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
                        //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected decimal digit or '+' or '-'\n", c, c);
                        ERROR(stderr, "Unknown character '%c' (0x%02x). Expected decimal digit or '+' or '-'", c, c)
                        return LEXICAL_ERROR;
                }
                break;
            case EXP_SIGN_S:
                if ('0' <= c && c <= '9') {
                    fsm_state = REAL_EXP_S;
                    String.add_char(lexeme, c);
                } else {
                    //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected decimal digit\n", c, c);
                    ERROR(stderr, "Unknown character '%c' (0x%02x). Expected decimal digit", c, c)
                    return LEXICAL_ERROR;
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
                        //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected printable character\n", c, c);
                        ERROR(stderr, "Unknown character '%c' (0x%02x). Expected printable character", c, c)
                        return LEXICAL_ERROR;
                }
                break;
            case STR_1_END_S:
                if (c == '"') {
                    fsm_state = STR_2_START_S;
                    String.add_char(lexeme, c);
                } else {
                    type = TOKEN_STRING_LITERAL;
                    attribute.string = verify_str(lexeme, false);
                    if (attribute.string == NULL) {
                        fprintf(stderr, "Error: Invalid string literal\n");
                        return LEXICAL_ERROR;
                    }
                    add_token(type, attribute, true);
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
                    //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected newline\n", c, c);
                    ERROR(stderr, "Unknown character '%c' (0x%02x). Expected newline", c, c)
                    return LEXICAL_ERROR;
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
                        //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected printable character\n", c, c);
                        ERROR(stderr, "Unknown character '%c' (0x%02x). Expected printable character", c, c)
                        return LEXICAL_ERROR;
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
                        //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected printable character\n", c, c);
                        ERROR(stderr, "Unknown character '%c' (0x%02x). Expected printable character", c, c)
                        return LEXICAL_ERROR;
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
                        //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected printable character\n", c, c);
                        ERROR(stderr, "Unknown character '%c' (0x%02x). Expected printable character", c, c)
                        return LEXICAL_ERROR;
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
                        //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected printable character\n", c, c);
                        ERROR(stderr, "Unknown character '%c' (0x%02x). Expected printable character", c, c)
                        return LEXICAL_ERROR;
                }
                break;
            case STR_2_END_S:
                type = TOKEN_STRING_LITERAL;
                attribute.string = verify_str(lexeme, true);
                if (attribute.string == NULL) {
                    fprintf(stderr, "Error: Invalid string literal\n");
                    return LEXICAL_ERROR;
                }
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
                        //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Unknown escape sequence. Expected '\"', 'n', 'r', 't', '\\', 'u'\n", c, c);
                        ERROR(stderr, "Unknown character '%c' (0x%02x). Unknown escape sequence. Expected '\"', 'n', 'r', 't', '\\', 'u'", c, c)
                        return LEXICAL_ERROR;
                }
                break;
            case U_SEC_S:
                if (c == '{') {
                    String.add_char(lexeme, c);
                    fsm_state = U_SEC_START_S;
                } else {
                    //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected '{'\n", c, c);
                    ERROR(stderr, "Unknown character '%c' (0x%02x). Expected '{'", c, c)
                    return LEXICAL_ERROR;
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
                        //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected hexadecimal digit\n", c, c);
                        ERROR(stderr, "Unknown character '%c' (0x%02x). Expected hexadecimal digit", c, c)
                        return LEXICAL_ERROR;
                }
                break;
            case U_SEC_NUM_S:
                if (count >= 8) {
                    //fprintf(stderr, "Too many hex digits in string escape sequence, maximum allowed is 8 digits\n");
                    ERROR(stderr, "Too many hex digits in string escape sequence, maximum allowed is 8 digits")
                    return LEXICAL_ERROR;
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
                        //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected hexadecimal digit or '}'\n", c, c);
                        ERROR(stderr, "Unknown character '%c' (0x%02x). Expected hexadecimal digit or '}'", c, c)
                        return LEXICAL_ERROR;
                }
                break;
                // comments
            case DIV_S:
                switch (c) {
                    case '/':
                        fsm_state = COM_SINGL_S;
                        break;
                    case '*':
                        fsm_state = COM_MULT_START_S;
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
                    fsm_state = COM_END_SINGL_S;
                }
                break;
            case COM_MULT_START_S:
                comment_cnt++;
                if (c == '*') {
                    fsm_state = COM_MULT_NEW_S;
                } else {
                    fsm_state = COM_MULT_S;
                }
                break;
            case COM_MULT_NEW_S:
                if (c == '/') {
                    fsm_state = COM_MULT_NEW_S;
                } else if (c == '*') {
                    fsm_state = COM_MULT_START_S;
                } else {
                    fsm_state = COM_MULT_S;
                }
                break;
            case COM_MULT_S:
                if (c == '/') {
                    fsm_state = COM_MULT_NEW_S;
                } else if (c == '*') {
                    fsm_state = COM_HALF_END_S;
                } else {
                    fsm_state = COM_MULT_S;
                }
                break;
            case COM_HALF_END_S:
                if (c == '*') {
                    fsm_state = COM_HALF_END_S;
                } else if (c == '/') {
                    fsm_state = COM_MULT_END_S;
                } else {
                    fsm_state = COM_MULT_S;
                }
                break;
            case COM_MULT_END_S:
                comment_cnt--;
                if (comment_cnt == 0) {
                    if (!isspace(control_char)) File.back_step();
                    c = ' ';
                    fsm_state = START_S;
                } else {
                    if (c == '/') {
                        fsm_state = COM_MULT_NEW_S;
                    } else {
                        fsm_state = COM_MULT_S;
                    }
                }
                break;
            case COM_END_SINGL_S:
                if (!isspace(control_char)) File.back_step();
                c = '\n';
                fsm_state = START_S;
                break;
            case LOGICAL_AND1_S:
                if (c == '&') {
                    fsm_state = LOGICAL_AND2_S;
                } else {
                    //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected '&'\n", c, c);
                    ERROR(stderr, "Unknown character '%c' (0x%02x). Expected '&'", c, c)
                    return LEXICAL_ERROR;
                }
                break;
            case LOGICAL_AND2_S:
                type = TOKEN_LOGICAL_AND;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case LOGICAL_OR1_S:
                if (c == '|') {
                    fsm_state = LOGICAL_OR2_S;
                }
                else {
                    //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected '|'\n", c, c);
                    ERROR(stderr, "Unknown character '%c' (0x%02x). Expected '|'", c, c)
                    return LEXICAL_ERROR;
                }
                break;
            case LOGICAL_OR2_S:
                type = TOKEN_LOGICAL_OR;
                add_token(type, attribute, false);
                fsm_state = START_S;
                break;
            case RANGE_START_S:
                if (c == '.') {
                    fsm_state = RANGE_CLOSED_S;
                }
                else {
                    //fprintf(stderr, "Error: Unknown character '%c' (0x%02x). Expected '.'\n", c, c);
                    ERROR(stderr, "Unknown character '%c' (0x%02x). Expected '.'", c, c)
                    return LEXICAL_ERROR;
                }
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
    String.dtor(lexeme);
    if (comment_cnt > 0) {
        fprintf(stderr, "Error: Unclosed multiline comment\nNumber of unclosed blocks: %d\n", comment_cnt);
        return LEXICAL_ERROR;
    }
    add_token(TOKEN_EOF, attribute, false);
    return SUCCESS;
}