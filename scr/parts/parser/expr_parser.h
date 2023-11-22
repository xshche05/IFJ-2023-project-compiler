#ifndef IFJ_PRJ_EXPR_PARSER_H
#define IFJ_PRJ_EXPR_PARSER_H

#include <stdbool.h>
#include "parser.h"

//bool parse_expr(bool step_back_first);



#define T_SIZE

typedef enum
{
    UNWRAP,                     // ID!
    NOT,                        // !ID
    ADD,			            // +
    SUB,			            // -
    MUL,			            // *
    DIV,			            // /
    EQUAL_TO,				    // ==
    NOT_EQUAL_TO,               // !=
    IS_NIL,                     // ??
    LESS_THAN,			        // <
    LESS_THAN_OR_EQUAL_TO,	    // <=
    GREATER_THAN_OR_EQUAL_TO,   // >=
    GREATER_THAN,			    // >
    AND,                        // &&
    OR,                         // ||
    LEFT_BRACKET,               // (
    RIGHT_BRACKET,	            // )
    IDENTIFIER,		            // ID
    INTEGER_LITERAL,		    // int
    REAL_LITERAL,	            // double
    STRING_LITERAL,             // string
    NIL_LITERAL,
    TRUE_LITERAL,               // TRUE
    FALSE_LITERAL,              // FALSE
    DOLLAR,			            // $
    NON_TERM		            // non-terminal
} Prec_table;

typedef enum
{
    INDEX_UNWRAP,            //0
    INDEX_NOT,               //1
    INDEX_ADD_SUB,           //2
    INDEX_MUL_DIV,           //3
    INDEX_NIL_COL,           //4
    INDEX_REL_OP,            //5
    INDEX_AND,               //6
    INDEX_OR,                //7
    INDEX_LEFT_BRACKET,      //8
    INDEX_IDENTIFIER,        //9
    INDEX_RIGHT_BRACKET,     //10
    INDEX_DOLLAR             //11
} table_index;

// 1 = SHIFT, 2 = REDUCE, 3 = EQUAL, 4 = ERROR
int table[T_SIZE][T_SIZE] ={
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // UNWRAP
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // NOT
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // ADD
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // SUB
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // MUL
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // DIV
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // EQUAL_TO
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // NOT_EQUAL_TO
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // IS_NIL
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // LESS_THAN
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // LESS_THAN_OR_EQUAL_TO
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // GREATER_THAN_OR_EQUAL_TO
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // GREATER_THAN
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // AND
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // OR
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // LEFT_BRACKET
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // RIGHT_BRACKET
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // IDENTIFIER
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // INTEGER_LITERAL
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // REAL_LITERAL
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // STRING_LITERAL
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // NIL_LITERAL
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // TRUE_LITERAL
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // FALSE_LITERAL
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}, // DOLLAR
        {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1}  // NON_TERM
};

// rules for bottom-up analysis
typedef enum ptRulese
{
    ID,             // E --> id
    LB_E_RB,        // E --> ( E )
    E_PLUS_E,       // E --> E + E
    E_MINUS_E,      // E --> E - E
    E_MULT_E,       // E --> E * E
    E_DIV_E,        // E --> E / E
    E_ASSIGN_E,     // E --> E = E
    E_GREATER_E,    // E --> E > E
    E_LESS_E,       // E --> E < E
    E_LEQ_E,        // E --> E <= E
    E_GEQ_E,        // E --> E >= E
    E_NOT_EQ_E,     // E --> E != E
    E_EQ_E,         // E --> E == E
    E_AND_E,        // E --> E && E
    E_OR_E,         // E --> E || E
    E_UNWRAP,       // E --> E !
    E_NOT,          // E --> ! E
    IF_E_NILL,      // E -> E?          TODO DON'T KNOW HOW TO WORK WITH THIS
    NILL_E,         // E -> E ?? E      TODO DON'T KNOW HOW TO WORK WITH THIS

    ERROR           // don't know this rule :(
} ptRule;




#endif //IFJ_PRJ_EXPR_PARSER_H
