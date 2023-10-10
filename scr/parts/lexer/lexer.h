#ifndef IFJ_PRJ_LEXER_H
#define IFJ_PRJ_LEXER_H

#include "../utils/utils.h"


typedef enum {
    TOKEN_IDENTIFIER,           //      S to [a-zA-Z_] to [a-zA-Z0-9_]
    TOKEN_OPERATOR,             //
    TOKEN_KEYWORD,              //
    TOKEN_LITERAL,              //
    TOKEN_PUNCTUATOR,           //
} token_type_t;

typedef enum {
    ASSIGNMENT,                 // =    S to =

    ADDITION,                   // +    S to +
    SUBTRACTION,                // -    S to -
    MULTIPLICATION,             // *    S to *
    DIVISION,                   // /    S to /

    LESS_THAN,                  // <    S to <
    LESS_THAN_OR_EQUAL_TO,      // <=   S to < to =
    GREATER_THAN,               // >    S to >
    GREATER_THAN_OR_EQUAL_TO,   // >=   S to > to =
    EQUAL_TO,                   // ==   S to = to =
    NOT_EQUAL_TO,               // !=   S to ! to =

    NILABLE,                    // ?    S to ?
    IS_NIL,                     // ??   S to ? to ?
    UNWRAP_NILABLE,             // .!   S to !

    // PREMIUM
    LOGICAL_AND,                // &&   S to & to &
    LOGICAL_OR,                 // ||   S to | to |
    LOGICAL_NOT,                // !.   S to !
} operator_type_t;

typedef enum {
    INTEGER_LITERAL,                    // int      S to [0-9]
    REAL_LITERAL,                       // real     S to [0-9] to . to [0-9]
                                //          S to [0-9] to . to [0-9] to e|E to [0-9]
                                //          S to [0-9] to . to [0-9] to e|E to +|- to [0-9]
    STRING_LITERAL,                     // string   S to " to [^"] to "
                                //          S to " to " to " to \n to [^"] to " to " to " to \n
    NIL_LITERAL,                        // nil

    // PREMIUM
    TRUE_LITERAL,                       // bool
    FALSE_LITERAL,                      // bool
} literal_type_t;

typedef enum {
    DOUBLE_TYPE,                // Double
    ELSE,                       // else
    FUNC,                       // func
    IF,                         // if
    INT_TYPE,                   // Int
    LET,                        // let
    RETURN,                     // return
    STRING_TYPE,                // String
    VAR,                        // var
    WHILE,                      // while

    // PREMIUM
    BOOL_TYPE,                  // Bool
} keyword_type_t;

typedef enum {
    LEFT_BRACKET,               // (        S to (
    RIGHT_BRACKET,              // )        S to )
    LEFT_BRACE,                 // {        S to {
    RIGHT_BRACE,                // }        S to }
    COMMA,                      // ,        S to ,
    COLON,                      // :        S to :
    SEMICOLON,                  // ;        S to ;
    ARROW,                      // ->       S to - to >
} punctuator_type_t;

typedef union {
    operator_type_t operator_type;
    literal_type_t literal_type;
    keyword_type_t keyword_type;
    punctuator_type_t punctuator_type;
} token_subtype_t;

typedef struct {
    token_type_t type;
    token_subtype_t subtype;
    string_t *lexeme;
} token_t;

typedef struct {
    token_t **array;
    int length;
    int allocated;
} token_array_t;

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
token_t *token_ctor(token_type_t type, token_subtype_t subtype, string_t *lexeme);

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
token_array_t *token_array_ctor();

/**
 * @brief deallocate token array structure
 *
 * @param token_array - pointer to token array structure
 */
void token_array_dtor(token_array_t *token_array);

/**
 * @brief add token to token array
 *
 * @param token_array - pointer to token array structure
 * @param token - pointer to token structure
 * @return 0 if success, non-zero otherwise
 */
int token_array_add(token_array_t *token_array, token_t *token);

/**
 * @brief convert source code to tokens
 *
 * @param file - pointer to file structure
 * @return pointer to token array structure
 */
token_array_t *source_code_to_tokens(file_t *file);

void token_print(token_t *token);

#endif //IFJ_PRJ_LEXER_H
