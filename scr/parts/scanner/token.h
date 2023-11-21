#ifndef IFJ_PRJ_TOKEN_H
#define IFJ_PRJ_TOKEN_H
#include "utils.h"
#include <stdbool.h>

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
    TOKEN_UNDERSCORE,                 // _
    // Operators
    TOKEN_ASSIGNMENT,                 // =
    TOKEN_CLOSED_RANGE,               // ..
    TOKEN_HALF_OPEN_RANGE,            // ..<
    // Literals
    TOKEN_REAL_LITERAL,
    TOKEN_STRING_LITERAL,
    TOKEN_NIL_LITERAL,
    TOKEN_TRUE_LITERAL,
    TOKEN_FALSE_LITERAL,
    TOKEN_INTEGER_LITERAL,
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
    TOKEN_IS_NIL,                     // ??
    TOKEN_UNWRAP_NILLABLE,             // ID!
    TOKEN_LOGICAL_AND,                // &&
    TOKEN_LOGICAL_OR,                 // ||
    TOKEN_LOGICAL_NOT,                // !ID
    // Punctuation
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_ARROW,
    TOKEN_EOF
} token_type_t;

typedef union {
    string_t *identifier;
    bool nillable;
    string_t *string;
    int integer;
    double real;
} token_attribute;

typedef struct {
    token_type_t type;
    token_attribute attribute;
    bool has_newline_after;
} token_t;

typedef struct {
    token_t **array;
    int length;
    int allocated;
    int current;
} token_array_t;

extern const struct token_interface Token;

extern const struct token_array_interface TokenArray;

/**
 * @brief Structure of token interface.
 *
 * @param ctor - Constructor
 * @param dtor - Destructor
 * @param print - Print token
 */
struct token_interface {
    token_t *(*ctor)(token_type_t type, token_attribute attribute, bool has_attribute);
    void (*dtor)(token_t *token);
    void (*print)(token_t *token);
};

/**
 * @brief Structure of token array interface.
 *
 * @param ctor - Constructor
 * @param dtor - Destructor
 * @param add - Add token to array
 * @param get - Get token from array
 * @param print - Print token array
 */
struct token_array_interface {
    int (*ctor)(void);
    void (*dtor)(void);
    int (*add)(token_t *token);
    token_t *(*curr)(void);
    token_t *(*next)(void);
    token_t *(*prev)(void);
    int (*total)(void);
    int (*set_nl_after)(void);
};
#endif //IFJ_PRJ_TOKEN_H
