#ifndef IFJ_PRJ_LEXER_H
#define IFJ_PRJ_LEXER_H

#include "utils.h"

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
    INTEGER,                    // int      S to [0-9]
    REAL,                       // real     S to [0-9] to . to [0-9]
                                //          S to [0-9] to . to [0-9] to e|E to [0-9]
                                //          S to [0-9] to . to [0-9] to e|E to +|- to [0-9]
    STRING,                     // string   S to " to [^"] to "
                                //          S to " to " to " to \n to [^"] to " to " to " to \n
    NIL,                        // nil

    // PREMIUM
    BOOL,                       // bool
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
    WHITESYMB_S,      // \n \t \r \v \f
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
 * @brief This function initializes token.
 *
 * @param type - Type of token
 * @param subtype - Subtype of token
 * @param lexeme - Lexeme of token
 * @return Pointer to token
 */
token_t *token_init(token_type_t type, token_subtype_t subtype, string_t *lexeme);

/**
 * @brief This function destroys token.
 *
 * @param token - Pointer to token
 */
void token_dtor(token_t *token);
/**
 * @brief This function initializes token array.
 *
 * @return Pointer to token array
 */
token_array_t *token_array_init();

/**
 * @brief This function destroys token array.
 *
 * @param token_array - Pointer to token array
 */
void token_array_dtor(token_array_t *token_array);

/**
 * @brief This function adds token to token array.
 *
 * @param token_array - Pointer to token array
 * @param token - Pointer to token
 * @return 0 if success, -1 otherwise
 */
int token_array_add(token_array_t *token_array, token_t *token);

/**
 * @brief This function initializes token array from source code file.
 *
 * @param file - Pointer to source code file
 * @return Pointer to token array
 */
token_array_t *source_code_to_tokens(file_t *file);



#endif //IFJ_PRJ_LEXER_H
