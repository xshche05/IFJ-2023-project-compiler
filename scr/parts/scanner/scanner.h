#ifndef IFJ_PRJ_SCANNER_H
#define IFJ_PRJ_SCANNER_H

#include <stdbool.h>
#include "../utils/utils.h"

typedef enum {
    // ID
    TOKEN_IDENTIFIER,
    // Keywords
    TOKEN_DOUBLE_TYPE,                // Double
    TOKEN_ELSE,                       // else
    TOKEN_FUNC,                       // func
    TOKEN_IF,                         // if
    TOKEN_INT_TYPE,                   // Int
    TOKEN_LET,                        // let
    TOKEN_RETURN,                     // return
    TOKEN_STRING_TYPE,                // String
    TOKEN_VAR,                        // var
    TOKEN_WHILE,                      // while
    TOKEN_BOOL_TYPE,                  // Bool
    TOKEN_FOR,                        // for
    TOKEN_IN,                         // in
    TOKEN_BREAK,                      // break
    TOKEN_CONTINUE,                   // continue
    // Operators
    TOKEN_ASSIGNMENT,                 // =
    TOKEN_ADDITION,                   // +
    TOKEN_SUBTRACTION,                // -
    TOKEN_MULTIPLICATION,             // *
    TOKEN_DIVISION,                   // /
    TOKEN_LESS_THAN,                  // <
    TOKEN_LESS_THAN_OR_EQUAL_TO,      // <=
    TOKEN_GREATER_THAN,               // >
    TOKEN_GREATER_THAN_OR_EQUAL_TO,   // >=
    TOKEN_EQUAL_TO,                   // ==
    TOKEN_NOT_EQUAL_TO,               // !=
    TOKEN_NILABLE,                    // ?
    TOKEN_IS_NIL,                     // ??
    TOKEN_UNWRAP_NILABLE,             // ID!
    TOKEN_LOGICAL_AND,                // &&
    TOKEN_LOGICAL_OR,                 // ||
    TOKEN_LOGICAL_NOT,                // !ID
    TOKEN_CLOSED_RANGE,               // ..
    TOKEN_HALF_OPEN_RANGE,            // ..<
    // Literals
    TOKEN_INTEGER_LITERAL,
    TOKEN_REAL_LITERAL,
    TOKEN_STRING_LITERAL,
    TOKEN_NIL_LITERAL,
    TOKEN_TRUE_LITERAL,
    TOKEN_FALSE_LITERAL,
    // Punctuation
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_ARROW,
} token_type_t;

typedef union {
    string_t *identifier;
    string_t *string;
    int integer;
    double real;
} token_attribute;

typedef struct {
    token_type_t type;
    token_attribute attribute;
} token_t;

typedef struct {
    token_t **array;
    int length;
    int allocated;
} token_array_t;

extern token_array_t *tokens;

typedef enum {
    START_S,          // S
    PLUS_S,           // +
    MUL_S,            // *
    EXCL_MARK_S,      // !
    NOT_EQUAL_S,      // !=
    NILABLE_S,        // ?
    IS_NIL_S,         // ??
    GREATER_S,        // >
    GREATER_EQUAL_S,  // >=
    LESS_S,           // <
    LESS_EQUAL_S,     // <=
    MINUS_S,          // -
    ARROW_S,          // ->
    ASSIGN_S,         // =
    EQUAL_S,          // ==
    LOGICAL_AND1_S,   // &
    LOGICAL_AND2_S,   // &&
    LOGICAL_OR1_S,    // |
    LOGICAL_OR2_S,    // ||
    BRACE_L_S,        // {
    BRACE_R_S,        // }
    BRACKET_L_S,      // (
    BRACKET_R_S,      // )
    COLON_S,          // :
    SEMICOLON_S,      // ;
    COMMA_S,          // ,
    INTEGER_S,        // 0-9
    ID_S,             // a-zA-Z_
    REAL_S,           // 0-9.
    REAL_NUM_S,       // 0-9.0-9
    REAL_E_S,         // 0-9.eE
    EXP_SIGN_S,       // 0-9.eE+-0-9
    REAL_EXP_S,       // 0-9.eE+-0-9
    STR_START_S,      // "
    STR_1_END_S,      // ""
    STR_2_START_S,    // """  _S, }
    STR_MULT_HALF_S,       // """\n
    STR_MULT_S,       // """\n
    STR_LF_S,         // """\n\n
    STR_2_E_S,        // """\n\n"
    STR_2_EE_S,       // """\n\n""
    STR_2_END_S,      // """\n\n"""
    ESC_S,            // \ _S, flag mult=false
    U_SEC_S,          // \u _S, flag mult=(false or true)
    U_SEC_START_S,    // \u{
    U_SEC_NUM_S,      // \u{0-9
    DIV_S,            // /
    COM_SINGL_S,      // //
    COM_MULT_S,       // /** _S,
    COM_HALF_END_S,   // /* _S, **
    COM_END_S       // /* , */
} fsm_state_t;

/**
 * @brief allocate and initialize token structure
 *
 * @param type - type of token
 * @param subtype - subtype of token
 * @param lexeme - lexeme of token
 * @return pointer to token structure
 */
token_t *token_ctor(token_type_t type, token_attribute attribute, bool has_attribute);

/**
 * @brief deallocate token structure
 *
 * @param token - pointer to token structure
 */
void token_dtor(token_t *token);

/**
 * @brief allocate and initialize token array structure
 *
 * @return pointer to token array structure
 */
int token_array_ctor();

/**
 * @brief deallocate token array structure
 *
 * @param token_array - pointer to token array structure
 */
void token_array_dtor();

/**
 * @brief add token to token array
 *
 * @param tokens - pointer to token array structure
 * @param token - pointer to token structure
 * @return 0 if success, non-zero otherwise
 */
int token_array_add(token_t *token);

/**
 * @brief convert source code to tokens
 *
 * @param file - pointer to file structure
 * @return pointer to token array structure
 */
int source_code_to_tokens();

void token_print(token_t *token);

#endif //IFJ_PRJ_SCANNER_H
