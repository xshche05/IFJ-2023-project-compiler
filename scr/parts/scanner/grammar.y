%token id type name underscore
%% /* LL(1) */
PROG : CODE 
| /*eps*/ ;

CODE : VAR_DECL '\n' CODE
| LET_DECL '\n' CODE
| FUNC_DECL '\n' CODE
| WHILE_LOOP '\n' CODE
| BRANCH '\n' CODE
| ASSINGMENT '\n' CODE
| FUNC_CALL '\n' CODE 
| RETURN;

RETURN : 'return' RET_EXPR;

RET_EXPR : EXPR
| /*eps*/ ;

VAR_DECL : 'var' id ':' type '=' EXPR
| 'var' id ':' type
| 'var' id '=' EXPR ;

LET_DECL : 'let' id ':' type '=' EXPR
| 'let' id ':' type
| 'let' id '=' EXPR ;

FUNC_DECL : 'func' id '(' PARAM_LIST ')' '->' type '{' CODE '}' '\n'
|'func' id '(' PARAM_LIST ')' '{' CODE '}' '\n';

PARAM_LIST : PARAM NEXT_PARAM
| /*eps*/ ;

PARAM : PARAM_NAME id ':' type ;
PARAM_NAME : id
| underscore ;
NEXT_PARAM : ',' PARAM NEXT_PARAM
| /*eps*/ ;


BRANCH : 'if' BR_EXPR '{' CODE '}' 'else' '{' CODE '}' ;
BR_EXPR : EXPR
| 'let' id ;

WHILE_LOOP : 'while' EXPR '{' CODE '}' ;

ASASSINGMENT : id '=' EXPR;

FUNC_CALL : id '(' CALL_PARAM_LIST ')'

CALL_PARAM_LIST : CALL_PARAM NEXT_CALL_PARAM
| /*eps*/ ;

CALL_PARAM : 