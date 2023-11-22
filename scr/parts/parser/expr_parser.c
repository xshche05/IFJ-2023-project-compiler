//
//// valid tokens for expression parser id-non-nil, int_literal, double_literal, str_literal, nil_literal, true_literal, false_literal
//// (, ), +, -, *, /, <, <=, >, >=, ==, !=, &&, ||, !_, _!
//
//#include "expr_parser.h"
//#include "token/token.h"
//#include "stack.h"
//
//
//// expression to reverse polish notation
//// returns 0 if success, 1 if error
//
//// operator priority_table
//static int get_op_priority(token_t *token) {
//    switch (token->type) {
//        case TOKEN_LOGICAL_NOT:
//            return 0;
//        case TOKEN_MULTIPLICATION:
//        case TOKEN_DIVISION:
//            return 1;
//        case TOKEN_ADDITION:
//        case TOKEN_SUBTRACTION:
//            return 2;
//        case TOKEN_EQUAL_TO:
//        case TOKEN_NOT_EQUAL_TO:
//        case TOKEN_LESS_THAN:
//        case TOKEN_LESS_THAN_OR_EQUAL_TO:
//        case TOKEN_GREATER_THAN:
//        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
//            return 3;
//        case TOKEN_LOGICAL_AND:
//            return 4;
//        case TOKEN_LOGICAL_OR:
//            return 5;
//        case TOKEN_UNWRAP_NILABLE:
//            return 100;
//        default:
//            return -1;
//    }
//}
//
//
//bool parse_expr() {
//    TokenArray.prev();
//    token_t *token;
//    token_t *last_expr_token = NULL;
//    bool cont = true;
//    stack_t *op_stack = Stack.init();
//    stack_t *out_stack = Stack.init();
//    token_t *tmp;
//    while (cont) {
//        token = TokenArray.next();
//        if (token == NULL) {
//            break;
//       \ }
//        switch (token->type) {
//            case TOKEN_IDENTIFIER:
//            case TOKEN_INTEGER_LITERAL:
//            case TOKEN_REAL_LITERAL:
//            case TOKEN_STRING_LITERAL:
//            case TOKEN_NIL_LITERAL:
//            case TOKEN_TRUE_LITERAL:
//            case TOKEN_FALSE_LITERAL:
//                Stack.push(out_stack, token);
//                last_expr_token = token;
//                break;
//            case TOKEN_LEFT_BRACKET:
//                // tmp = Stack.top(out_stack);
//                if (last_expr_token != NULL && last_expr_token->type == TOKEN_IDENTIFIER) {
//                    // function call
//                    Stack.push(out_stack, token);
//                    while (last_expr_token->type != TOKEN_RIGHT_BRACKET) {
//                        last_expr_token = TokenArray.next();
//                        Stack.push(out_stack, last_expr_token);
//                    }
//                } else {
//                    // expression
//                    Stack.push(op_stack, token);
//                    last_expr_token = token;
//                }
//                break;
//            case TOKEN_RIGHT_BRACKET: // pop operators from op_stack to out_stack until left bracket
//                while (true) {
//                    token_t *op = Stack.top(op_stack);
//                    Stack.pop(op_stack);
//                    if (op == NULL) {
//                        fprintf(stderr, "Error: Missing left bracket.\n");
//                        return false;
//                    }
//                    if (op->type == TOKEN_LEFT_BRACKET) {
//                        break;
//                    }
//                    Stack.push(out_stack, op);
//                }
//                last_expr_token = token;
//                break;
//            case TOKEN_UNWRAP_NILABLE:
//                // switch id flag to unnillable
//                tmp = Stack.top(out_stack);
//                if (tmp->type != TOKEN_IDENTIFIER && tmp->type != TOKEN_RIGHT_BRACKET) {
//                    fprintf(stderr, "Error: Unwrap nilable operator must be after ID or func call.\n");
//                    return false;
//                }
//                Stack.push(out_stack, token);
//                break;
//            case TOKEN_LOGICAL_NOT:
////                Stack.push(out_stack, token);
////                break;
//            case TOKEN_MULTIPLICATION:
//            case TOKEN_DIVISION:
//            case TOKEN_ADDITION:
//            case TOKEN_SUBTRACTION:
//            case TOKEN_EQUAL_TO:
//            case TOKEN_NOT_EQUAL_TO:
//            case TOKEN_LESS_THAN:
//            case TOKEN_LESS_THAN_OR_EQUAL_TO:
//            case TOKEN_GREATER_THAN:
//            case TOKEN_GREATER_THAN_OR_EQUAL_TO:
//            case TOKEN_LOGICAL_AND:
//            case TOKEN_LOGICAL_OR:
//                // pop operators from op_stack to out_stack until operator with lower priority
//                while (true) {
//                    token_t *op = Stack.top(op_stack);
//                    if (op == NULL || op->type == TOKEN_LEFT_BRACKET) {
//                        break;
//                    }
//                    if (get_op_priority(op) >= get_op_priority(token)) {
//                        break;
//                    }
//                    Stack.pop(op_stack);
//                    Stack.push(out_stack, op);
//                }
//                Stack.push(op_stack, token);
//                last_expr_token = token;
//                break;
//            case TOKEN_NEWLINE:
//                continue;
//            default:
//                cont = false;
//                TokenArray.prev();
//                break;
//        }
//    }
//    if (get_op_priority(last_expr_token) != -1) {
//        fprintf(stderr, "Error: Unexpected operator at the end of expression.\n");
//        return false;
//    }
//    // pop operators from op_stack to out_stack
//    while (true) {
//        token_t *op = Stack.top(op_stack);
//        if (op == NULL) {
//            break;
//        }
//        Stack.pop(op_stack);
//        Stack.push(out_stack, op);
//    }
//    // reverse out_stack
//    stack_t *tmp_stack = Stack.init();
//    while (true) {
//        token_t *op = Stack.top(out_stack);
//        if (op == NULL) {
//            break;
//        }
//        Stack.pop(out_stack);
//        Stack.push(tmp_stack, op);
//    }
//    // print out_stack
//    while (true) {
//        token_t *op = Stack.top(tmp_stack);
//        if (op == NULL) {
//            break;
//        }
//        Stack.pop(tmp_stack);
//        Token.print(op);
//    }
//    TokenArray.prev();
//    lookahead = TokenArray.next()->type;
//    printf("\n\n\n");
//    return true;
//    // LAST TOKEN CHECK IN PARSER!!!!
//}
//

#include "scanner/token.h"
#include "stack.h"
//stack_t *stack = stack_init();

#define T_SIZE 12

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

int table[T_SIZE][T_SIZE] ={
                {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 }, // UNWRAP
                {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 }, // NOT
                {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 }, // ADD_SUB
                {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 }, // MUL_DIV
                {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 }, // NIL_COL
                {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 }, // REL_OP
                {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 }, // AND
                {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 }, // OR
                {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 }, // LEFT_BRACKET
                {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 }, // IDENTIFIER
                {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 }, // RIGHT_BRACKET
                {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 }}; // DOLLAR

/*int scope(bool* stop_found)
{
    struct stack_item* tmp = stack_top(&stack);
    int count = 0;

   while (tmp != NULL)
    {
        if (tmp->data != (void*)'$')
        {
            *stop_found = false;
            count++;
        }
        else
        {
            *stop_found = true;
            break;
        }

        tmp = tmp->down;
    }

    return count;
}*/

table_index get_table_index(token_t* token)
{
    switch (token->type)
    {
        case TOKEN_UNWRAP_NILLABLE:
            return INDEX_UNWRAP;

        case TOKEN_LOGICAL_NOT:
            return INDEX_NOT;

        case TOKEN_ADDITION:
        case TOKEN_SUBTRACTION:
            return INDEX_ADD_SUB;

        case TOKEN_MULTIPLICATION:
        case TOKEN_DIVISION:
            return INDEX_MUL_DIV;

        case TOKEN_IS_NIL:
            return INDEX_NIL_COL;

        case TOKEN_LESS_THAN:
        case TOKEN_GREATER_THAN:
        case TOKEN_LESS_THAN_OR_EQUAL_TO:
        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
        case TOKEN_EQUAL_TO:
        case TOKEN_NOT_EQUAL_TO:
            return INDEX_REL_OP;

        case TOKEN_LOGICAL_AND:
            return INDEX_AND;

        case TOKEN_LOGICAL_OR:
            return INDEX_OR;

        case TOKEN_LEFT_BRACKET:
            return INDEX_LEFT_BRACKET;

        case TOKEN_RIGHT_BRACKET:
            return INDEX_RIGHT_BRACKET;

        case TOKEN_IDENTIFIER:
        case TOKEN_REAL_LITERAL:
        case TOKEN_STRING_LITERAL:
        case TOKEN_INTEGER_LITERAL:
        case TOKEN_NIL_LITERAL:
        case TOKEN_TRUE_LITERAL:
        case TOKEN_FALSE_LITERAL:
            return INDEX_IDENTIFIER;

        default:
            return INDEX_DOLLAR;
    }
}

/*Prec_table get_symbol_from_token(token_t* token)
{
    switch (token->type)
    {
        case TOKEN_UNWRAP_NILLABLE:
            return UNWRAP;

        case TOKEN_LOGICAL_NOT:
            return NOT;

        case TOKEN_ADDITION:
            return ADD;
        case TOKEN_SUBTRACTION:
            return SUB;

        case TOKEN_MULTIPLICATION:
            return MUL;
        case TOKEN_DIVISION:
            return DIV;

        case TOKEN_IS_NIL:
            return IS_NIL;

        case TOKEN_LESS_THAN:
            return LESS_THAN;
        case TOKEN_GREATER_THAN:
            return GREATER_THAN;
        case TOKEN_LESS_THAN_OR_EQUAL_TO:
            return LESS_THAN_OR_EQUAL_TO;
        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
            return GREATER_THAN_OR_EQUAL_TO;
        case TOKEN_EQUAL_TO:
            return EQUAL_TO;
        case TOKEN_NOT_EQUAL_TO:
            return NOT_EQUAL_TO;

        case TOKEN_LOGICAL_AND:
            return AND;

        case TOKEN_LOGICAL_OR:
            return OR;

        case TOKEN_LEFT_BRACKET:
            return LEFT_BRACKET;

        case TOKEN_RIGHT_BRACKET:
            return RIGHT_BRACKET;

        case TOKEN_IDENTIFIER:
            return IDENTIFIER;
        case TOKEN_REAL_LITERAL:
            return REAL_LITERAL;
        case TOKEN_STRING_LITERAL:
            return STRING_LITERAL;
        case TOKEN_INTEGER_LITERAL:
            return INTEGER_LITERAL;
        case TOKEN_NIL_LITERAL:
            return NIL_LITERAL;
        case TOKEN_TRUE_LITERAL:
            return TRUE_LITERAL;
        case TOKEN_FALSE_LITERAL:
            return FALSE_LITERAL;

        default:
            return DOLLAR;
    }
}*/

int expression(token_t* token) {
    stack_t *stack = Stack.init();

    Stack.push(stack, (void *) DOLLAR);
    void *top_stack_terminal;
    Prec_table actual_symbol;

    bool success = false;

}
