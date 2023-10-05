//
// Created by Spagetik on 02.10.2023.
//

#ifndef IFJ_PRJ_LEXER_H
#define IFJ_PRJ_LEXER_H

#include "utils.h"

typedef enum {
    TOKEN_IDENTIFIER,           //
    TOKEN_OPERATOR,             //
    TOKEN_KEYWORD,              //
    TOKEN_LITERAL,              //
    TOKEN_PUNCTUATOR,           //
} token_type_t;

typedef enum {
    ASSIGNMENT,                 // =

    ADDITION,                   // +
    SUBTRACTION,                // -
    MULTIPLICATION,             // *
    DIVISION,                   // /

    LESS_THAN,                  // <
    LESS_THAN_OR_EQUAL_TO,      // <=
    GREATER_THAN,               // >
    GREATER_THAN_OR_EQUAL_TO,   // >=
    EQUAL_TO,                   // ==
    NOT_EQUAL_TO,               // !=

    NILABLE,                    // ?
    IS_NIL,                     // ??
    UNWRAP_NILABLE,             // ...!

    // PREMIUM
    LOGICAL_AND,                // &&
    LOGICAL_OR,                 // ||
    LOGICAL_NOT,                // !
} operator_type_t;

typedef enum {
    INTEGER,                    // int
    REAL,                       // real
    STRING,                     // string
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
    LEFT_PARENTHESIS,           // (
    RIGHT_PARENTHESIS,          // )
    LEFT_BRACE,                 // {
    RIGHT_BRACE,                // }
    COMMA,                      // ,
    COLON,                      // :
    SEMICOLON,                  // ;
    ARROW,                      // ->
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

#endif //IFJ_PRJ_LEXER_H
